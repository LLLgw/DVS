#ifndef MESSAGE_H
#define MESSAGE_H

#pragma once
#include <QByteArray>

typedef enum
{
    FUNC_CONTROL    = 0x0001,
    FUNC_QUERY      = 0x0002,
    FUNC_DATA       = 0x0003, /*数据码*/
}func_t;
/*功能码*/

typedef enum
{
    CMD_CONTROL_TEST                = 0x0001,
    CMD_CONFIG_SAMPLEPOINTS         = 0x0002,
    CMD_CONFIG_PULSE_FREQUENCY      = 0x0004,
    CMD_CONTROL_AVERAGE             = 0x0008,
    CMD_CONFIG_SAMPLE_CHANNELS      = 0x0009,
    CMD_CONFIG_SAMPLE_DELAYPOINTS   = 0x0010,
    CMD_CONFIG_PULSE_WIDTH          = 0x0011,
    CMD_CONFIG_AVERAGE_NUMBER       = 0x0020,
    CMD_CONTROL_DIFFERENCE          = 0x0021,
    CMD_CONFIG_SAMPLE_FREQUENCY     = 0x0022,
    CMD_CONFIG_ADC_BIAS_VOLTAGE     = 0x0023,
    CMD_CONFIG_ADC_TRIGGER_SOURCE   = 0x0025,
}cmd_t;
/*命令码*/

typedef enum
{
    SCMD_DEV_LOG_REQ = 0x01,
    SCMD_DEV_LOG_OK = 0x02,
    SCMD_DEV_LOG_NOK = 0x03,
    SCMD_APP_LOG_IN_NOTIFY = 0x04,
    SCMD_APP_LOG_IN_ACK = 0x05,
    SCMD_APP_LOG_OUT_NOTIFY = 0x06,
    SCMD_APP_LOG_OUT_ACK = 0x07,
    SCMD_CONFIG_PULSE_WIDTH_REQ = 0x10,
    SCMD_CONFIG_PULSE_WIDTH_ACK = 0x11,
    SCMD_CONFIG_PULSE_FREQUENCY_REQ = 0x12,
    SCMD_CONFIG_PULSE_FREQUENCY_ACK = 0x13,
    SCMD_CONFIG_AVERAGE_NUMBER_REQ = 0x14,
    SCMD_CONFIG_AVERAGE_NUMBER_ACK = 0x15,
    SCMD_CONFIG_SAMPLEPOINTS_NUMBER_REQ = 0x16,
    SCMD_CONFIG_SAMPLEPOINTS_NUMBER_ACK = 0x17,
    SCMD_CONFIG_SAMPLE_FREQUENCY_REQ = 0x18,
    SCMD_CONFIG_SAMPLE_FREQUENCY_ACK = 0x19,
    SCMD_CONFIG_SPECTRUM_LOW_FREQUENCY_REQ = 0x1A,
    SCMD_CONFIG_SPECTRUM_LOW_FREQUENCY_ACK = 0x1B,
    SCMD_CONFIG_SPECTRUM_HIGH_FREQUENCY_REQ = 0x1C,
    SCMD_CONFIG_SPECTRUM_HIGH_FREQUENCY_ACK = 0x1D,
    SCMD_START_TEST_REQ = 0x30,
    SCMD_START_TEST_ACK = 0x31,
    SCMD_STOP_TEST_REQ = 0x32,
    SCMD_STOP_TEST_ACK = 0x33,
    SCMD_ENABLE_GET_RMS_REQ = 0x34,
    SCMD_ENABLE_GET_RMS_ACK = 0x35,
    SCMD_DISABLE_GET_RMS_REQ = 0x36,
    SCMD_DISABLE_GET_RMS_ACK = 0x37,
    // 【新增】设置上报数据长度指令
    SCMD_SET_UPLOAD_DATA_LEN_REQ = 0x38,
    SCMD_SET_UPLOAD_DATA_LEN_ACK = 0x39,
    SCMD_SET_RESOLUTION_REQ = 0x40,
    SCMD_SET_RESOLUTION_ACK = 0x41,
    // 【新增】设置截断距离指令
    SCMD_SET_TRUNCATE_DIST_REQ = 0x42,
    SCMD_SET_TRUNCATE_DIST_ACK = 0x43,
    SCMD_SET_TCP_UPSTREAM_REQ = 0x44,
    SCMD_SET_TCP_UPSTREAM_ACK = 0x45,
    SCMD_SET_UDP_REPORT_TARGET_REQ = 0x46,
    SCMD_SET_UDP_REPORT_TARGET_ACK = 0x47,
    SCMD_HEARTBEAT_REQ = 0x50,
    SCMD_HEARTBEAT_ACK = 0x51,
    SCMD_DEVICE_REBOOT_REQ = 0x52,
    SCMD_DEVICE_REBOOT_ACK = 0x53,
    SCMD_OTA_UPDATE_REQ = 0x54,
    SCMD_OTA_UPDATE_ACK = 0x55,
    SCMD_SET_SD_RAW_RECORD_REQ = 0x56,
    SCMD_SET_SD_RAW_RECORD_ACK = 0x57,
    SCMD_QUERY_SD_RAW_RECORD_REQ = 0x58,
    SCMD_QUERY_SD_RAW_RECORD_ACK = 0x59,
    SCMD_QUERY_SD_RAW_FILE_LIST_REQ = 0x5A,
    SCMD_QUERY_SD_RAW_FILE_LIST_ACK = 0x5B,
    // 【新增】查询SD卡原始数据文件下载请求指令
    SCMD_PZT_LOCATION_DATA = 0x60,
} svrcmd_t;

typedef struct ptofCommand {
    char    header[6];      // 头标识
    quint16 funcCode;       // 功能码
    quint16 cmdCode;        // 命令码
    quint32 paramLen;       // 参数长度
    char    reserved[2];    // 保留字段
    qint64  param;          // 参数
}ptofcmdstruct_t;
/*pc to fpga命令结构体*/

typedef struct ftopCommand
{
    char    header[6];      // 头标识
    quint16 funcCode;       // 功能码
    char    reserved[2];    // 保留字段
    quint16 dataLen;        // 参数长度
    quint16 cmdCode;        // 命令码
    int16_t  result;         // 参数
}ftopcmdstruct_t;
/*fpga to pc命令结构体*/

typedef struct SeverControlCommand {
    quint8  header;         // 头标识
    quint8  frameLen;       // 帧长
    quint8  cmdCode;        // 命令ID
    quint8  paramLen;       // 参数长度
    quint32 param;          // 参数
    char    reserved[36];   // 保留字段
}svrcmdstruct_t;

QByteArray buildFpgaCommand(func_t funcCode, cmd_t cmdCode, int16_t param);
bool parseFpgaCommand(const QByteArray &data, ftopcmdstruct_t &cmd);
QByteArray buildServerCommand(svrcmd_t cmdCode, quint32 param);
bool parseServerCommand(const QByteArray &data, svrcmdstruct_t &cmd);

QByteArray buildServerCommand2(svrcmd_t cmdCode, quint32 param1, quint32 param2);

#endif // MESSAGE_H
