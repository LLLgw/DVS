#include "dataacquisition.h"
#include "message.h"
#include "GlobalVars.h"
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QVector>
#include <QVariant>
#include <QDebug>
#include <QThread>
#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QStorageInfo>
#include <QTextStream>
#include <algorithm>
#include <chrono>
#ifdef Q_OS_LINUX
#include <QFile>
#include <cerrno>
#include <cstring>
#include <sched.h>
#endif

/*采集前准备*/
DataAcquisition::DataAcquisition(std::shared_ptr<SharedBuffer> shared,QObject *parent)
    : QObject{parent},m_shared(std::move(shared))
{
    fpgaAddress = QHostAddress("192.168.137.2"); /*fpga地址*/
    fpgaDataPort = 6788;                         /*本地数据端口*/

    const int rows = GlobalVars::signalSamplePoints; //已改动
    const int cols = GlobalVars::lineSamplePoints; // 注意：这里要确保构造时已确定
    m_buffers[0] = QVector<QVector<int16_t>>(rows, QVector<int16_t>(cols));
    m_buffers[1] = QVector<QVector<int16_t>>(rows, QVector<int16_t>(cols));
    m_buffers[2] = QVector<QVector<int16_t>>(rows, QVector<int16_t>(cols));
    //m_buffers[3] = QVector<QVector<int16_t>>(rows, QVector<int16_t>(cols));
    m_currentLineBuffer = QVector<int16_t>(cols, -2000);       /*列缓冲*/
    m_lineAccumBuffer = QVector<int32_t>(cols, 0);             /*列累积缓冲*/
    m_lineAvgCount = 0;

    // 只做一次初始化填充值
    for (auto &buf : m_buffers) {
        for (auto &row : buf) {
            std::fill(row.begin(), row.end(), -2000);
        }
    }

    resetAcquisitionState(false); /*初始化状态*/
    setupDataSocket();

    writeSdRecordStatus("idle");
}

DataAcquisition::~DataAcquisition()
{
    m_sdCmdPollStopRequested.store(true, std::memory_order_relaxed);
    if (m_sdCmdPollThread.joinable()) {
        m_sdCmdPollThread.join();
    }
    stopSdRecording(true);
}

void DataAcquisition::startAcquisition()
{
#ifdef Q_OS_LINUX
    // Keep UDP draining off the CPU handling eth0 IRQs; RPS handles protocol work on CPU3.
    cpu_set_t cpuSet;
    CPU_ZERO(&cpuSet);
    CPU_SET(1, &cpuSet);
    if (sched_setaffinity(0, sizeof(cpuSet), &cpuSet) != 0) {
        qWarning() << "Failed to bind acquisition thread to CPU1:" << std::strerror(errno);
    }

    QFile rpsFile("/sys/class/net/eth0/queues/rx-0/rps_cpus");
    if (rpsFile.open(QIODevice::WriteOnly)) {
        rpsFile.write("8\n"); // CPU3
    } else {
        qWarning() << "Failed to set eth0 RPS to CPU3:" << rpsFile.errorString();
    }
#endif
    ensureSdCommandPoller();
    pollSdRecordCommand();
    resetAcquisitionState(true);
    m_running = true;
}

void DataAcquisition::stopAcquisition()
{
    m_running = false;
    pollSdRecordCommand();
    resetAcquisitionState(true);
}

void DataAcquisition::ensureSdCommandPoller()
{
    if (m_sdCmdPollThread.joinable())
        return;

    m_sdCmdPollStopRequested.store(false, std::memory_order_relaxed);
    m_sdCmdPollThread = std::thread([this]() {
        while (!m_sdCmdPollStopRequested.load(std::memory_order_relaxed)) {
            pollSdRecordCommand();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });
}

/*开始写入sd卡*/
bool DataAcquisition::startSdRecording(const QString& targetDir)
{
    QString cleanDir = targetDir.trimmed();
    if (cleanDir.isEmpty()) {
        cleanDir = "/mnt/sdcard/dvs_raw";
    }

    QDir dir(cleanDir);
    if (!dir.exists() && !dir.mkpath(".")) {
        m_sdLastError = QString("mkdir failed: %1").arg(cleanDir);
        writeSdRecordStatus("start failed");
        return false;
    }

    if (!cleanupSdRawDir(cleanDir, m_sdMaxSingleFileBytes)) {
        m_sdLastError = QString("insufficient sd space: %1").arg(cleanDir);
        writeSdRecordStatus("start failed");
        return false;
    }

    if (m_sdRecording.load(std::memory_order_relaxed) || m_sdStopping.load(std::memory_order_relaxed)) {
        stopSdRecording(true);
    }

    if (m_sdWriterThread.joinable()) {
        m_sdWriterStopRequested.store(true, std::memory_order_relaxed);
        m_sdQueueCond.notify_all();
        m_sdWriterThread.join();
    }

    if (m_sdRawFile.isOpen()) {
        m_sdRawFile.close();
    }

    {
        std::lock_guard<std::mutex> lock(m_sdQueueMutex);
        m_sdRawQueue.clear();
    }

    const QString fileName = QString("raw_%1_i16le.bin")
                                 .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    const QString fullPath = dir.filePath(fileName);

    m_sdRawFile.setFileName(fullPath);
    if (!m_sdRawFile.open(QIODevice::WriteOnly)) {
        m_sdLastError = QString("open failed: %1").arg(fullPath);
        writeSdRecordStatus("start failed");
        return false;
    }

    m_sdCurrentDir = cleanDir;
    m_sdCurrentFilePath = fullPath;
    m_sdLinesWritten.store(0, std::memory_order_relaxed);
    m_sdBytesWritten.store(0, std::memory_order_relaxed);
    m_sdDroppedLines.store(0, std::memory_order_relaxed);
    m_sdWriteKBps.store(0, std::memory_order_relaxed);
    m_sdAutoStoppedByFileLimit.store(false, std::memory_order_relaxed);
    m_sdLastError.clear();
    m_sdStopping.store(false, std::memory_order_relaxed);
    m_sdRecording.store(true, std::memory_order_relaxed);
    m_sdWriterStopRequested.store(false, std::memory_order_relaxed);
    m_sdWriterRunning.store(true, std::memory_order_relaxed);
    m_sdWriterThread = std::thread(&DataAcquisition::sdWriterLoop, this);

    writeSdRecordStatus("recording");
    qInfo() << "SD record started:" << m_sdCurrentFilePath;
    return true;
}

void DataAcquisition::requestStopSdRecordingAsync()
{
    if (!m_sdRecording.load(std::memory_order_relaxed) &&
        !m_sdWriterRunning.load(std::memory_order_relaxed)) {
        writeSdRecordStatus("stopped");
        return;
    }

    m_sdRecording.store(false, std::memory_order_relaxed);
    m_sdStopping.store(true, std::memory_order_relaxed);
    m_sdAutoStoppedByFileLimit.store(false, std::memory_order_relaxed);
    m_sdWriterStopRequested.store(true, std::memory_order_relaxed);
    m_sdQueueCond.notify_all();
    writeSdRecordStatus("stopping");
}

void DataAcquisition::stopSdRecording(bool keepStatusMessage)
{
    m_sdRecording.store(false, std::memory_order_relaxed);
    if (!keepStatusMessage) {
        m_sdAutoStoppedByFileLimit.store(false, std::memory_order_relaxed);
    }
    m_sdStopping.store(true, std::memory_order_relaxed);
    m_sdWriterStopRequested.store(true, std::memory_order_relaxed);
    m_sdQueueCond.notify_all();

    if (m_sdWriterThread.joinable()) {
        m_sdWriterThread.join();
    }

    if (m_sdRawFile.isOpen()) {
        m_sdRawFile.flush();
        m_sdRawFile.close();
    }

    {
        std::lock_guard<std::mutex> lock(m_sdQueueMutex);
        m_sdRawQueue.clear();
    }

    m_sdWriterRunning.store(false, std::memory_order_relaxed);
    m_sdWriteKBps.store(0, std::memory_order_relaxed);
    m_sdStopping.store(false, std::memory_order_relaxed);
    m_sdRecording.store(false, std::memory_order_relaxed);
    if (!keepStatusMessage) {
        writeSdRecordStatus("stopped");
    }
}

void DataAcquisition::writeSdRecordStatus(const QString& statusMsg)
{
    const QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    auto writeOneStatusFile = [&](const QString& path) {
        QFile statusFile(path);
        if (!statusFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            return;
        }

        QTextStream out(&statusFile);
        QString state = "stopped";
        if (m_sdStopping.load(std::memory_order_relaxed)) {
            state = "stopping";
        } else if (m_sdRecording.load(std::memory_order_relaxed)) {
            state = "recording";
        }
        out << "state=" << state << "\n";
        out << "message=" << (statusMsg.isEmpty() ? "ok" : statusMsg) << "\n";
        out << "dir=" << m_sdCurrentDir << "\n";
        out << "file=" << (m_sdCurrentFilePath.isEmpty() ? "-" : m_sdCurrentFilePath) << "\n";
        out << "lines=" << m_sdLinesWritten.load(std::memory_order_relaxed) << "\n";
        out << "bytes=" << m_sdBytesWritten.load(std::memory_order_relaxed) << "\n";
        {
            std::lock_guard<std::mutex> lock(m_sdQueueMutex);
            out << "queued_lines=" << m_sdRawQueue.size() << "\n";
        }
        out << "dropped_lines=" << m_sdDroppedLines.load(std::memory_order_relaxed) << "\n";
        out << "write_kbps=" << m_sdWriteKBps.load(std::memory_order_relaxed) << "\n";
        out << "line_sample_points=" << GlobalVars::lineSamplePoints << "\n";
        out << "sample_format=int16_le" << "\n";
        out << "updated=" << now << "\n";
        out << "last_error=" << (m_sdLastError.isEmpty() ? "-" : m_sdLastError) << "\n";
    };

    // 主路径放在 /run，兼容输出一份到 /tmp 便于排查
    writeOneStatusFile(m_sdStatusPath);
    writeOneStatusFile("/tmp/dvs_sd_record_status");
}

/*命令文件轮询*/
void DataAcquisition::pollSdRecordCommand()
{
    QString cmdPath;
    QString cmdLine;

    const QStringList cmdPaths = { m_sdCmdPath, QStringLiteral("/tmp/dvs_sd_record_cmd") };
    for (const QString& path : cmdPaths) {
        QFile cmdFile(path);
        if (!cmdFile.exists()) {
            continue;
        }

        if (!cmdFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            m_sdLastError = QString("cannot read cmd file: %1").arg(path);
            writeSdRecordStatus("cmd read failed");
            return;
        }

        const QByteArray rawCmd = cmdFile.readAll();
        cmdLine = QString::fromUtf8(rawCmd).trimmed();
        cmdFile.close();
        cmdPath = path;
        break;
    }

    if (cmdPath.isEmpty()) {
        return;
    }

    if (cmdLine.isEmpty()) {
        // 可能在 shell 重定向写入窗口中读到了空文件，不立即删除，避免丢命令
        return;
    }

    qInfo() << "SD cmd(" << cmdPath << "):" << cmdLine;

    if (cmdLine.startsWith("start")) {
        QString targetDir = "/mnt/sdcard/dvs_raw";
        if (cmdLine.size() > 5) {
            targetDir = cmdLine.mid(5).trimmed();
        }
        if (!startSdRecording(targetDir)) {
            qWarning() << "SD record start failed:" << m_sdLastError;
        }
        QFile::remove(cmdPath);
        return;
    }

    if (cmdLine == "stop") {
        requestStopSdRecordingAsync();
        qInfo() << "SD record stop requested";
        QFile::remove(cmdPath);
        return;
    }

    if (cmdLine == "status") {
        writeSdRecordStatus("status");
        QFile::remove(cmdPath);
        return;
    }

    m_sdLastError = QString("unknown command: %1").arg(cmdLine);
    writeSdRecordStatus("unknown command");
    QFile::remove(cmdPath);
}

/*设置sd卡录制*/
void DataAcquisition::setSdRawRecording(quint32 enabled)
{
    if (enabled == 1) {
        if (m_sdRecording.load(std::memory_order_relaxed)) {
            emit sdRawRecordSetResult(1);
            return;
        }
        if (m_sdStopping.load(std::memory_order_relaxed)) {
            emit sdRawRecordSetResult(0);
            return;
        }
        emit sdRawRecordSetResult(startSdRecording(m_sdCurrentDir) ? 1 : 0);
        return;
    }

    if (enabled == 0) {
        if (!m_sdRecording.load(std::memory_order_relaxed) &&
            !m_sdWriterRunning.load(std::memory_order_relaxed)) {
            m_sdAutoStoppedByFileLimit.store(false, std::memory_order_relaxed);
            writeSdRecordStatus("stopped");
            emit sdRawRecordSetResult(1);
            return;
        }
        requestStopSdRecordingAsync();
        emit sdRawRecordSetResult(1);
        return;
    }

    emit sdRawRecordSetResult(0);
}

void DataAcquisition::querySdRawRecordStatus()
{
    writeSdRecordStatus("status");
    emit sdRawRecordStatus(currentSdRecordStatus());
}

void DataAcquisition::querySdRawFileList(quint32 count)
{
    int limit = static_cast<int>(count);
    if (limit < 1)
        limit = 1;
    if (limit > 10)
        limit = 10;

    QDir dir(m_sdCurrentDir);
    if (!dir.exists()) {
        emit sdRawFileListEnd();
        return;
    }

    const QFileInfoList files = dir.entryInfoList(QStringList() << "raw_*.bin", QDir::Files, QDir::Time);
    int sent = 0;
    for (const QFileInfo& file : files) {
        const quint64 sizeBytes = static_cast<quint64>(file.size());
        const quint32 sizeMb = static_cast<quint32>((sizeBytes + 1024ULL * 1024ULL - 1) / (1024ULL * 1024ULL));
        emit sdRawFileListItem(sizeMb, file.fileName());
        ++sent;
        if (sent >= limit)
            break;
    }
    emit sdRawFileListEnd();
}

void DataAcquisition::writeRawLineToSd(const QVector<int16_t>& rawLine)
{
    if (!m_sdRecording.load(std::memory_order_relaxed) || !m_sdWriterRunning.load(std::memory_order_relaxed)) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m_sdQueueMutex);
        if (static_cast<int>(m_sdRawQueue.size()) >= m_sdMaxQueuedLines) {
            m_sdRawQueue.pop_front();
            m_sdDroppedLines.fetch_add(1, std::memory_order_relaxed);
        }
        m_sdRawQueue.push_back(rawLine);
    }

    m_sdQueueCond.notify_one();
}

/*写入线程*/
void DataAcquisition::sdWriterLoop()
{
    quint64 bytesSampled = 0;
    auto sampleStart = std::chrono::steady_clock::now();

    while (true) {
        std::deque<QVector<int16_t>> localBatch;
        {
            std::unique_lock<std::mutex> lock(m_sdQueueMutex);
            /*等数据*/
            m_sdQueueCond.wait(lock, [this]() {
                return m_sdWriterStopRequested.load(std::memory_order_relaxed) || !m_sdRawQueue.empty();
            });

            if (m_sdWriterStopRequested.load(std::memory_order_relaxed) && m_sdRawQueue.empty()) {
                break;
            }

            const int takeLines = std::min(static_cast<int>(m_sdRawQueue.size()), m_sdWriteBatchLines);
            for (int i = 0; i < takeLines; ++i) {
                localBatch.push_back(std::move(m_sdRawQueue.front()));
                m_sdRawQueue.pop_front();
            }
        }

        if (localBatch.empty()) {
            continue;
        }

        QByteArray batchBytes;
        batchBytes.reserve(static_cast<int>(localBatch.size()) * GlobalVars::lineSamplePoints * static_cast<int>(sizeof(int16_t)));
        for (const QVector<int16_t>& line : localBatch) {
            batchBytes.append(reinterpret_cast<const char*>(line.constData()), line.size() * static_cast<int>(sizeof(int16_t)));
        }

        if (!m_sdRawFile.isOpen()) {
            m_sdLastError = "sd file closed unexpectedly";
            m_sdRecording.store(false, std::memory_order_relaxed);
            break;
        }

        const qint64 written = m_sdRawFile.write(batchBytes);
        if (written != batchBytes.size()) {
            m_sdLastError = QString("write failed: %1").arg(m_sdRawFile.errorString());
            qWarning() << "SD record write failed:" << m_sdLastError;
            m_sdRecording.store(false, std::memory_order_relaxed);
            break;
        }

        const quint64 totalLines = m_sdLinesWritten.fetch_add(static_cast<quint64>(localBatch.size()), std::memory_order_relaxed)
                                 + static_cast<quint64>(localBatch.size());
        const quint64 totalBytes = m_sdBytesWritten.fetch_add(static_cast<quint64>(written), std::memory_order_relaxed)
                                 + static_cast<quint64>(written);
        bytesSampled += static_cast<quint64>(written);

        if (totalBytes >= m_sdMaxSingleFileBytes) {
            m_sdAutoStoppedByFileLimit.store(true, std::memory_order_relaxed);
            m_sdLastError = "single file reached 2048MB";
            m_sdRecording.store(false, std::memory_order_relaxed);
            m_sdWriterStopRequested.store(true, std::memory_order_relaxed);
            break;
        }

        const auto now = std::chrono::steady_clock::now();
        const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - sampleStart).count();
        if (elapsedMs >= 1000) {
            const double sec = static_cast<double>(elapsedMs) / 1000.0;
            m_sdWriteKBps.store(static_cast<quint32>((bytesSampled / 1024.0) / sec), std::memory_order_relaxed);
            bytesSampled = 0;
            sampleStart = now;
        }

        if ((totalLines & 0x1FF) == 0) {
            m_sdRawFile.flush();
            writeSdRecordStatus("recording");
        }
    }

    if (m_sdRawFile.isOpen()) {
        m_sdRawFile.flush();
        m_sdRawFile.close();
    }
    m_sdWriteKBps.store(0, std::memory_order_relaxed);
    m_sdStopping.store(false, std::memory_order_relaxed);
    m_sdRecording.store(false, std::memory_order_relaxed);
    m_sdWriterRunning.store(false, std::memory_order_relaxed);
    writeSdRecordStatus(m_sdAutoStoppedByFileLimit.load(std::memory_order_relaxed)
                        ? "file limit reached" : "stopped");
}

quint32 DataAcquisition::currentSdRecordStatus() const
{
    if (m_sdAutoStoppedByFileLimit.load(std::memory_order_relaxed))
        return 4;
    if (m_sdStopping.load(std::memory_order_relaxed))
        return 3;
    if (m_sdRecording.load(std::memory_order_relaxed))
        return 1;
    if (!m_sdLastError.isEmpty() && m_sdLastError != "single file reached 2048MB")
        return 2;
    return 0;
}

bool DataAcquisition::cleanupSdRawDir(const QString& targetDir, quint64 requiredFreeBytes)
{
    QDir dir(targetDir);
    if (!dir.exists())
        return false;

    QFileInfoList files = dir.entryInfoList(QStringList() << "raw_*.bin", QDir::Files, QDir::Time | QDir::Reversed);
    quint64 totalBytes = 0;
    for (const QFileInfo& file : files) {
        totalBytes += static_cast<quint64>(file.size());
    }

    auto removeOldest = [&]() -> bool {
        if (files.isEmpty())
            return false;
        const QFileInfo oldest = files.takeFirst();
        const quint64 size = static_cast<quint64>(oldest.size());
        if (!QFile::remove(oldest.absoluteFilePath())) {
            m_sdLastError = QString("remove old file failed: %1").arg(oldest.fileName());
            return false;
        }
        totalBytes = (totalBytes > size) ? (totalBytes - size) : 0;
        return true;
    };

    while (totalBytes > m_sdMaxDirBytes) {
        if (!removeOldest())
            return false;
    }

    QStorageInfo storage(targetDir);
    storage.refresh();
    while (storage.isValid() && storage.bytesAvailable() >= 0 &&
           static_cast<quint64>(storage.bytesAvailable()) < requiredFreeBytes) {
        if (!removeOldest())
            return false;
        storage.refresh();
    }

    return true;
}

/*初始化采集状态*/
void DataAcquisition::resetAcquisitionState(bool clearPendingDatagrams)
{
    firstPacketOK = false;
    m_shared->m_currentBuffer.store(0);//改
    m_currentCycle.store(0);
    m_lineAvgCount = 0;

    /*判断当前行是否为lineSamplePoints*/
    if (m_currentLineBuffer.size() != GlobalVars::lineSamplePoints) {
        m_currentLineBuffer = QVector<int16_t>(GlobalVars::lineSamplePoints, -2000);
    } else {
        std::fill(m_currentLineBuffer.begin(), m_currentLineBuffer.end(), -2000);
    }

    if (m_lineAccumBuffer.size() != GlobalVars::lineSamplePoints) {
        m_lineAccumBuffer = QVector<int32_t>(GlobalVars::lineSamplePoints, 0);
    } else {
        std::fill(m_lineAccumBuffer.begin(), m_lineAccumBuffer.end(), 0);
    }

    m_packetsProcessed = 0;
    m_lineMissingFrameCount = 0;
    m_lineDuplicateFrameCount = 0;
    m_lineBadCount = 0;
    m_currentLineHasDuplicateFrame = false;
    if (m_lineFrameSeen.size() != GlobalVars::kTotalFramesPerCycle) {
        m_lineFrameSeen = QVector<quint8>(GlobalVars::kTotalFramesPerCycle, 0);
    } else {
        std::fill(m_lineFrameSeen.begin(), m_lineFrameSeen.end(), 0);
    }
    m_perfTimerStarted = false;

    if (clearPendingDatagrams && dataSocket) {
        while (dataSocket->hasPendingDatagrams()) {
            QByteArray pending;
            pending.resize(static_cast<int>(dataSocket->pendingDatagramSize()));
            dataSocket->readDatagram(pending.data(), pending.size());
        }
    }
}

/*创建数据接收udp socket */
void DataAcquisition::setupDataSocket()
{
    dataSocket = new QUdpSocket(this);

    //qDebug() << "Receive buffer size:" << socket->socketOption(QAbstractSocket::ReceiveBufferSizeSocketOption).toInt();
    // Request a large UDP receive buffer for 10000-point acquisition bursts.
    // Linux doubles SO_RCVBUF internally, so this needs net.core.rmem_max >= 128 MB
    // to take full effect; the actual value is printed below for verification.
    
    /*包接收缓冲区为64MB*/
    dataSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, QVariant(64 * 1024 * 1024));
    if (!dataSocket->bind(QHostAddress::Any, fpgaDataPort))
    {
        qWarning() << "Failed to bind dataUDP socket";
        return;
    }

    qDebug() << "UDP receiver started on port 6788, receive buffer:"
             << dataSocket->socketOption(QAbstractSocket::ReceiveBufferSizeSocketOption).toInt();
}

/*数据接收处理*/
void DataAcquisition::processDatagram()
{
    if (!m_running.load(std::memory_order_relaxed)) {
        return;
    }

    static const int HEADER_SIZE = 16;                                     /*每帧前16个字节*/
    static const int SAMPLE_SIZE = 2;                                      /*每个sample占2个字节*/
    static const int SAMPLES_PER_FRAME = GlobalVars::kMaxSamplesPerFrame;  /*每帧的最大样本数*/

    const int totalFramesPerCycle = GlobalVars::kTotalFramesPerCycle;      /*总帧数*/
    const int remainingSamples = GlobalVars::kRemainingSamples;            /*最后一帧的点数*/
    const int lineSamplePoints = GlobalVars::lineSamplePoints;             /*每行采样点数*/
    const bool rawDataDisplayEnable = GlobalVars::rawDataDisplayEnable;

    if (!m_perfTimerStarted) {
        m_perfTimer.start();
        m_perfTimerStarted = true;
    }

    static thread_local QByteArray datagramBuffer;

    static int16_t count_frame_ = 0;

    while (dataSocket->hasPendingDatagrams())
    {
        const qint64 datagramSize = dataSocket->pendingDatagramSize();          /*包大小*/
        datagramBuffer.resize(static_cast<int>(datagramSize));

        const qint64 bytesRead = dataSocket->readDatagram(datagramBuffer.data(), datagramSize);/*取包数据*/
        if (bytesRead != datagramSize) {
            continue;
        }
        m_packetsProcessed++;

        const uchar* data = reinterpret_cast<const uchar*>(datagramBuffer.constData());

        quint16 funcCode = (static_cast<quint16>(data[6]) << 8) | data[7];          /*取功能码*/
        quint16 endFlag = (static_cast<quint16>(data[10]) << 8) | data[11];         /*数据标志*/
        quint16 packetNumber = (static_cast<quint16>(data[12]) << 8) | data[13];    /*数据包序号*/
        quint16 packetLen = (static_cast<quint16>(data[14]) << 8) | data[15];       /*数据包长度*/

        if (bytesRead != packetLen) {
            continue;
        }

        /*判断是否为数据包*/
        if (funcCode != FUNC_DATA) {
            firstPacketOK = false;
            std::fill(m_lineFrameSeen.begin(), m_lineFrameSeen.end(), 0);
            continue;
        }

        /*判断正常包序号*/
        if (packetNumber == 0 || packetNumber > totalFramesPerCycle) {
            firstPacketOK = false;
            std::fill(m_lineFrameSeen.begin(), m_lineFrameSeen.end(), 0);
            continue;
        }

        if (m_lineFrameSeen.size() != totalFramesPerCycle) {
            m_lineFrameSeen = QVector<quint8>(totalFramesPerCycle, 0);
        }

        /*第一个包*/
        if (packetNumber == 1) {
            if (firstPacketOK) {
                int missingFrames = 0;
                for (quint8 seen : m_lineFrameSeen) {
                    if (!seen) {
                        ++missingFrames;
                    }
                }
                if (missingFrames > 0) {
                    ++m_lineBadCount;
                    m_lineMissingFrameCount += missingFrames;
                }
            }
            std::fill(m_lineFrameSeen.begin(), m_lineFrameSeen.end(), 0);
            m_currentLineHasDuplicateFrame = false;
            firstPacketOK = true;

        } else if (!firstPacketOK) {
            // A line must begin with its first packet; late fragments cannot be assigned safely.
            continue;
        }

        const int frameIndex = packetNumber - 1;  /*数据帧索引*/
        
        /*记录每行的帧*/
        if (m_lineFrameSeen[frameIndex]) {
            ++m_lineDuplicateFrameCount;
            m_currentLineHasDuplicateFrame = true;
        } else {
            m_lineFrameSeen[frameIndex] = 1;    
        }

        // 7. 优化：使用位运算判断是否为最后一帧
        const bool isLastFrame = (packetNumber == totalFramesPerCycle);
        if (isLastFrame && endFlag != 0x1100) {
            firstPacketOK = false;
            std::fill(m_lineFrameSeen.begin(), m_lineFrameSeen.end(), 0);
            continue;
        }

        // 8. 优化：简化三元运算符
        /*计算当前帧的样本数*/
        const int currentFrameSamples = isLastFrame ? (remainingSamples > 0 ? remainingSamples : SAMPLES_PER_FRAME) : SAMPLES_PER_FRAME;

        // 9. 优化：预计算起始列
        const int startCol = (packetNumber - 1) * SAMPLES_PER_FRAME;

        // 10. 优化：展开循环，减少循环开销
        const int loopEnd = currentFrameSamples;
        const int sampleDataStart = HEADER_SIZE; /*帧头部（除开data外）*/

        if (m_currentLineBuffer.size() < lineSamplePoints) {
            m_currentLineBuffer.resize(lineSamplePoints);
            std::fill(m_currentLineBuffer.begin(), m_currentLineBuffer.end(), -2000);
        }

        int16_t* bufferPtr = m_currentLineBuffer.data();

        // 11. 优化：使用指针算术，避免多次operator[]
        const uchar* sampleData = data + sampleDataStart;

        // 处理采样数据
        int i = 0;

        // 展开4次循环，提高性能
        for (; i + 3 < loopEnd; i += 4) {
            int idx0 = i * SAMPLE_SIZE;
            int idx1 = (i + 1) * SAMPLE_SIZE;
            int idx2 = (i + 2) * SAMPLE_SIZE;
            int idx3 = (i + 3) * SAMPLE_SIZE;

            int col0 = startCol + i;
            int col1 = startCol + i + 1;
            int col2 = startCol + i + 2;
            int col3 = startCol + i + 3;

            bufferPtr[col0] = static_cast<int16_t>((sampleData[idx0] << 8) | sampleData[idx0 + 1]);
            bufferPtr[col1] = static_cast<int16_t>((sampleData[idx1] << 8) | sampleData[idx1 + 1]);
            bufferPtr[col2] = static_cast<int16_t>((sampleData[idx2] << 8) | sampleData[idx2 + 1]);
            bufferPtr[col3] = static_cast<int16_t>((sampleData[idx3] << 8) | sampleData[idx3 + 1]);
        }

        // 处理剩余样本
        for (; i < loopEnd; ++i) {
            int idx = i * SAMPLE_SIZE;
            int col = startCol + i;
            bufferPtr[col] = static_cast<int16_t>((sampleData[idx] << 8) | sampleData[idx + 1]);
        }

        // 12. 优化：最后一帧处理
        if (isLastFrame)
        {
            int missingFrames = 0;
            for (quint8 seen : m_lineFrameSeen) {
                if (!seen) {
                    ++missingFrames;
                }
            }
            if (missingFrames > 0) {
                m_lineMissingFrameCount += missingFrames;
            }

            const bool lineComplete = (missingFrames == 0);
            if (!lineComplete) {
                ++m_lineBadCount;
                firstPacketOK = false;
                std::fill(m_lineFrameSeen.begin(), m_lineFrameSeen.end(), 0);
                continue;
            }

            // 检查缓冲区大小
            if (m_currentLineBuffer.size() >= lineSamplePoints)
            {
                QVector<int16_t> rawLineCopy;
                const bool needRawLineCopy = m_sdRecording.load(std::memory_order_relaxed)
                                             || (rawDataDisplayEnable && m_currentCycle == 0);
                if (needRawLineCopy) {
                    rawLineCopy.resize(lineSamplePoints);
                    memcpy(rawLineCopy.data(), bufferPtr, lineSamplePoints * sizeof(int16_t));
                }

                /*写入SD卡*/
                if (m_sdRecording.load(std::memory_order_relaxed))
                    writeRawLineToSd(rawLineCopy);
                    //  emit rmsRawDataReady(rawLineCopy);

                if (rawDataDisplayEnable && m_currentCycle == 0)
                    emit displayRawDataReady(rawLineCopy);

                /*缓冲区已经填满，但是没有处理*/
                const int writeCycle = m_currentCycle.load(std::memory_order_relaxed);
                if (writeCycle >= GlobalVars::signalSamplePoints) {
                    //std::fill(m_lineAccumBuffer.begin(), m_lineAccumBuffer.end(), 0);
                    m_lineAvgCount = 0;
                    m_currentCycle.store(0, std::memory_order_relaxed);
                    firstPacketOK = false;
                    continue;
                }

                auto& dstRow = m_buffers[m_shared->m_currentBuffer][writeCycle];
                if (dstRow.size() != lineSamplePoints) {
                    dstRow.resize(lineSamplePoints);
                }

                //buffer是否空闲
                if( m_shared ->state[m_shared->m_currentBuffer] != bufferState::Processing)
                {   
                    m_shared -> state[m_shared->m_currentBuffer] = bufferState::Receiving;
                    //数据放至buffer
                    for(int col = 0 ; col < lineSamplePoints; col++){
                        dstRow[col] = bufferPtr[col];
                    }
                }
                else
                {
                    count_frame_ ++;
                    firstPacketOK = false;
                    std::fill(m_lineFrameSeen.begin(), m_lineFrameSeen.end(), 0);
                    qint64 elapsed = m_perfTimer.elapsed();
                    m_perfTimer.restart();
                    continue;
                }

            }
            else
            {
                firstPacketOK = false;
                std::fill(m_lineFrameSeen.begin(), m_lineFrameSeen.end(), 0);
                continue;
            }

            // Do not let this line's packet bitmap make a later partial line look complete.
            firstPacketOK = false;
            std::fill(m_lineFrameSeen.begin(), m_lineFrameSeen.end(), 0);

            // 每得到一条软件平均后的 1xN，再推进一次cycle
            int newCycle = m_currentCycle.fetch_add(1, std::memory_order_relaxed) + 1;

            // 检查是否完成所有采集
            if (newCycle >= GlobalVars::signalSamplePoints)
            {
                int readyBuffer = m_shared->m_currentBuffer;
                int otherBuffer = readyBuffer + 1;

                
                if(otherBuffer > 2){ 
                    otherBuffer = 0;
                }

                m_currentCycle.store(0, std::memory_order_relaxed);

                firstPacketOK = false;

                m_shared -> state[readyBuffer] = bufferState::Processing;

                // 发送原始数据
                emit specRawDataReady(&m_buffers[readyBuffer],readyBuffer);

                qint64 elapsed = m_perfTimer.elapsed();
                if (m_lineBadCount > 0 || m_lineDuplicateFrameCount > 0) {
                    qWarning() << "DataAcquisition: Processed" << m_packetsProcessed << "Packets in" << elapsed
                               << "ms | bad lines:" << m_lineBadCount
                               << "| missing frames:" << m_lineMissingFrameCount
                               << "| duplicate frames:" << m_lineDuplicateFrameCount;
                } else {
                    qDebug() << "DataAcquisition: Processed" << m_packetsProcessed << "Packets in" << elapsed << "ms" << "count_frame_: "<< count_frame_;
                }

                // 切换到另一个缓冲继续接收
                {
                    std::lock_guard<std::mutex> lock(m_shared->mutex);
                    m_shared->m_currentBuffer = otherBuffer;
                }

                //改
                count_frame_ = 0;

                m_packetsProcessed = 0;
                m_lineMissingFrameCount = 0;
                m_lineDuplicateFrameCount = 0;
                m_lineBadCount = 0;
                m_perfTimer.restart();
            }
        }
    }
}

void DataAcquisition::onProcessorBusy(bool busy)
{
    m_processorBusy.store(busy);
    if (busy)
        m_busyTimer.restart();
}

