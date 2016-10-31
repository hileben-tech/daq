#ifndef SCKCMODBUS_H
#define SCKCMODBUS_H

#include <QMutex>
#include <QObject>
#include <QStringList>
#include "buildopts.h"
#include "modbusrtuprotocol.h"


#define SCKCMODBUS_PORT_NAME        "/dev/ttyO4"
#define SCKCMODBUS_BAUDRATE         19200//9600//19200//38400
#define SCKCMODBUS_SLAVE_ADDRESS    3


/* SCK-C-MODBUS Info */
#define REG_SCKCM_STATUS                      0x0480
#define REG_SCKCM_PACKET_ERR_NUM              0x0492
#define REG_SCKCM_TEMP                        0x04A0
#define REG_SCKCM_MODULE_ID                   0x04B1
#define REG_SCKCM_SERIAL_NUM                  0x04B2
#define REG_SCKCM_MODULE_NAME                 0x04B6

/* Channel Data Info */
#define REG_SCKCM_CH_DATA_START               0x0000
#define REG_SCKCM_CH_DATA_OFFSET              0x0B

#define REG_SCKCM_CH_DATA_STATUS_OFFSET       0x00
#define REG_SCKCM_CH_DATA_CURRENT_OFFSET      0x01
#define REG_SCKCM_CH_DATA_TEMP_OFFSET         0x09
#define REG_SCKCM_CH_DATA_VOLT_OFFSET         0x0A

#define REG_SCKCM_CH_DATA_STATUS_CNT          1
#define REG_SCKCM_CH_DATA_CURRENT_CNT         8
#define REG_SCKCM_CH_DATA_TEMP_CNT            1
#define REG_SCKCM_CH_DATA_VOLT_CNT            1


/* Channel Module Info */
#define REG_SCKCM_CH_MI_START                 0x0080

#define REG_SCKCM_CH_MI_MODULE_ID_OFFSET      0x31
#define REG_SCKCM_CH_MI_SERIAL_NUM_OFFSET     0x32
#define REG_SCKCM_CH_MI_MODULE_NAME_OFFSET    0x35

#define REG_SCKCM_CH_MI_MODULE_ID_CNT         1
#define REG_SCKCM_CH_MI_SERIAL_NUM_CNT        4
#define REG_SCKCM_CH_MI_MODULE_NAME_CNT       16

/* Status Info */
#define STAT_SCKCM_CC               (1 << 4)
#define STAT_SCKCM_NC               (1 << 5)
#define STAT_SCKCM_ER               (1 << 6)
#define STAT_SCKCM_DIS              (1 << 7)


class ChannelInfo
{
public:
    bool    connected;
    QString moduleName;
    QString serialNum;
    QString current;
    QString voltage;
};

class SckCModbus : public QObject
{
    Q_OBJECT

    ModbusRTUProtocol *modbus;
    QString port;
    int slaveAddress;
    bool connected;

    QString     commModuleName;     /* Communication Module name */
    QString     commSerialNum;      /* Communication Module Serial Number */
    ChannelInfo channel[8];         /* Communication Module Port Ts' info */

    QMutex mutexData;

private:
    void printDataHex(QString msg, char *data, int size);
    void swapByteOrder(char *srcBuf, char *destBuf, int bytes);

public:
    explicit SckCModbus(QObject *parent = 0);
    ~SckCModbus();

    int  connect(QString portName, int baudrate);
    void disconnect();
    bool isConnected();
    int  setSlave(int slaveAddr);
    int  checkChConnection();
    bool isChConnected(int ch);

    int     readChInfo(int ch);
    QString readStatus(int ch);
    QString readSerialNumber(int ch);
    QString readModuleName(int ch);
    QString readCurrent(int ch);
    QString readTemperature(int ch);
    QString readVoltage(int ch);

    void setSerialNum(int ch, QString serialNum);
    void setModuleName(int ch, QString moduleName);
    void setCurrent(int ch, QString curr);
    void setVoltage(int ch, QString volt);

    QString getChannelStatus();
    QString getSerialNum(int ch);
    QString getModuleName(int ch);
    QString getCurrent(int ch);
    QString getVoltage(int ch);


protected:

signals:

public slots:

};

#endif // SCKCMODBUS_H
