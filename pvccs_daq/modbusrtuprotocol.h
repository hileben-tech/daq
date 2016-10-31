/******************************************************************************
 * Filename    : modbusrtuprotocol.h
 * Description : Header of ModbusRTUProtocol class
 * Create Date : 2016.06.24
 * Writter     : Moon, Sung Won
 *****************************************************************************/
#ifndef MODBUSRTUPROTOCOL_H
#define MODBUSRTUPROTOCOL_H

#include <QObject>
#include <QString>
#include <modbus.h>
#include "serportrs485.h"


#define MODBUS_RTU_FCODE_READ_HOLDING_REG   0x03


class ModbusRTUProtocol : public QObject
{
    SerPortRS485 *rs485;
    int  readDelay;
    bool isConnect;

private:
    int sendRequestMsg(int slaveAddr, int funcCode, int startAddr, int regNum);
    int checkCRC(char *buf, int size);
    void printDataHex(char *data, int size);

public:
    explicit ModbusRTUProtocol(QObject *parent = 0);
    virtual ~ModbusRTUProtocol();

    int  openPort(QString portName, int baudRate);
    int  closePort();
    bool isConnected();
    int  readHoldingRegs(int slaveAddr, int startAddr, int regNum, char *readVal);

protected:

signals:

public slots:

};

#endif // MODBUSRTUPROTOCOL_H
