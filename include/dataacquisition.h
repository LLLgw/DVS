#ifndef DATAACQUISITION_H
#define DATAACQUISITION_H

#include <QObject>
#include <QUdpSocket>
#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QString>
#include <condition_variable>
#include <deque>
#include <thread>
#include "atomic"
#include "mutex"
#include <cstdint>
#include "buffer_state.h"

class DataAcquisition : public QObject
{
    Q_OBJECT
public:
    explicit DataAcquisition(std::shared_ptr<SharedBuffer> shared,QObject *parent = nullptr);
    ~DataAcquisition();
    void setupDataSocket();

    QUdpSocket *dataSocket;

public slots:
    void startAcquisition();
    void stopAcquisition();
    void processDatagram();
    void onProcessorBusy(bool busy);
    void pollSdRecordCommand();
    void setSdRawRecording(quint32 enabled);
    void querySdRawRecordStatus();
    void querySdRawFileList(quint32 count);

signals:
    void specRawDataReady(QVector<QVector<int16_t>>* rawDataPtr,int bufferIndex);//改
    void rmsRawDataReady(const QVector<int16_t>& rawData);  // 发送RMS处理原始数据
    void displayRawDataReady(const QVector<int16_t>& rawData);
    void transferedDataReady(const QByteArray& packet);
    void sdRawRecordSetResult(quint32 result);
    void sdRawRecordStatus(quint32 status);
    void sdRawFileListItem(quint32 sizeMb, const QString& fileName);
    void sdRawFileListEnd();

private:
    QVector<QVector<int16_t>> m_buffers[3]; //改
    QVector<int16_t> m_currentLineBuffer;
    QVector<int32_t> m_lineAccumBuffer;
    int m_lineAvgCount = 0;
    //std::atomic<int> m_currentBuffer{0};
    std::atomic<bool> m_running{false};
    std::atomic<int> m_currentCycle{0};
    //std::mutex m_bufferMutex;               // 保护缓冲区切换

    std::atomic<bool> m_processorBusy{false};
    QElapsedTimer m_busyTimer;
    const qint64 PROCESSING_TIMEOUT = 1200;

    bool firstPacketOK;
    QHostAddress fpgaAddress;
    quint16 fpgaDataPort;

    int m_packetsProcessed = 0;
    QVector<quint8> m_lineFrameSeen;
    bool m_currentLineHasDuplicateFrame = false;
    int m_lineMissingFrameCount = 0;
    int m_lineDuplicateFrameCount = 0;
    int m_lineBadCount = 0;
    QElapsedTimer m_perfTimer;
    bool m_perfTimerStarted = false;

    std::thread m_sdCmdPollThread;
    std::atomic<bool> m_sdCmdPollStopRequested{false};
    QFile m_sdRawFile;
    QString m_sdCmdPath = "/run/dvs_sd_record_cmd";
    QString m_sdStatusPath = "/run/dvs_sd_record_status";
    QString m_sdCurrentDir = "/mnt/sdcard/dvs_raw";
    QString m_sdCurrentFilePath;
    QString m_sdLastError;
    std::atomic<bool> m_sdRecording{false};
    std::atomic<bool> m_sdStopping{false};
    std::atomic<quint64> m_sdLinesWritten{0};
    std::atomic<quint64> m_sdBytesWritten{0};
    std::atomic<quint64> m_sdDroppedLines{0};
    std::atomic<quint32> m_sdWriteKBps{0};
    std::atomic<bool> m_sdAutoStoppedByFileLimit{false};

    std::deque<QVector<int16_t>> m_sdRawQueue;
    std::mutex m_sdQueueMutex;
    std::condition_variable m_sdQueueCond;
    std::thread m_sdWriterThread;
    std::atomic<bool> m_sdWriterStopRequested{false};
    std::atomic<bool> m_sdWriterRunning{false};
    const int m_sdWriteBatchLines = 64;
    const int m_sdMaxQueuedLines = 2048;
    const quint64 m_sdMaxSingleFileBytes = 2048ULL * 1024ULL * 1024ULL;
    const quint64 m_sdMaxDirBytes = 100ULL * 1024ULL * 1024ULL * 1024ULL;

    void resetAcquisitionState(bool clearPendingDatagrams);
    void ensureSdCommandPoller();
    bool startSdRecording(const QString& targetDir);
    void requestStopSdRecordingAsync();
    void stopSdRecording(bool keepStatusMessage = false);
    void writeSdRecordStatus(const QString& statusMsg = QString());
    void writeRawLineToSd(const QVector<int16_t>& rawLine);
    void sdWriterLoop();
    quint32 currentSdRecordStatus() const;
    bool cleanupSdRawDir(const QString& targetDir, quint64 requiredFreeBytes);

    std::shared_ptr<SharedBuffer> m_shared;
};

#endif // DATAACQUISITION_H
