/******************************************************************************
 * Filename    : modbusrtu.cpp
 * Description : Class for Modbus RTU protocol interface
 * Create Date : 2016.06.24
 * Writter     : Moon, Sung Won
 *****************************************************************************/
#include <QString>
#include <QDebug>
#include "modbusrtuprotocol.h"
#include "serportrs485.h"
#include "utility.h"


ModbusRTUProtocol::ModbusRTUProtocol(QObject *parent)
{
    rs485 = new SerPortRS485;
    isConnect = false;
}

ModbusRTUProtocol::~ModbusRTUProtocol()
{
    closePort();
    delete rs485;
    rs485 = NULL;
}

int ModbusRTUProtocol::openPort(QString portName, int baudRate)
{
    if (rs485->openPort(portName, baudRate) != 0)
    {
        qDebug() << "[ModbusRTUProtocol::openPort] Open serial failed." << portName;
        return -1;
    }

    if (baudRate == 9600)
        readDelay = 100;
    else if (baudRate == 19200)
        readDelay = 50;
    else
        readDelay = 20;

    isConnect = true;
    return 0;
}

int ModbusRTUProtocol::closePort()
{
    if (rs485)
    {
        rs485->closePort();
        qDebug() << "[ModbusRTUProtocol::closePort] closed.";
    }

    isConnect = false;
    return 0;
}

bool ModbusRTUProtocol::isConnected()
{
    return isConnect;
}

int ModbusRTUProtocol::sendRequestMsg(int slaveAddr, int funcCode, int startAddr, int regNum)
{
    char  req[16];
    short crc;

    req[0] = (uchar)slaveAddr;
    req[1] = (uchar)funcCode;
    req[2] = (uchar)(startAddr >> 8);
    req[3] = (uchar)(startAddr & 0x000000ff);
    req[4] = (uchar)(regNum >> 8);
    req[5] = (uchar)(regNum & 0x000000ff);

    crc = Utility::crc16(req, 6);
    req[6] = crc >> 8;
    req[7] = crc & 0x00FF;

    return (rs485->writeBytes(req, 8));
}

/* Return byte count of register data. readVal contains only Data of registers. */
int ModbusRTUProtocol::readHoldingRegs(int slaveAddr, int startAddr, int regNum, char *readVal)
{
    int i, readBytes, readSize = 0;
    char readBuf[256];

    if (isConnected() == false)
    {
        return -1;
    }

    if (sendRequestMsg(slaveAddr, MODBUS_RTU_FCODE_READ_HOLDING_REG, startAddr, regNum) == -1)
    {
        return -1;
    }

    //Utility::msleep(readDelay); /* Need some delay. */
    //Utility::msleep(35); /* Need some delay. */
    //Utility::msleep(15); /* Need some delay. */
    Utility::msleep(50); /* Need some delay. */

    /* Response :
     * - First 0(1) : SCK-C-MODBUS 의경우 첫 바이트에 항상 '0'이 들어옴. StartByte??
     * - SlaveAddress(1) FunctionCode(1) ByteCount(1)
     * - Data(2 * regNum)
     * - CRC(2)
     */
    int respHeaderBytes = 1 + 3;
    int respCRCBytes    = 2;
    readSize = (regNum * 2) + respHeaderBytes + respCRCBytes;

    readBytes = rs485->readBytes(readBuf, readSize);

    if (readBytes == readSize)
    {
        if (checkCRC(&readBuf[1], (readBytes - 1)) == 0) /* Ignore first '0' */
        {
            readSize -= respHeaderBytes + respCRCBytes;
            memcpy(readVal, &readBuf[respHeaderBytes], readSize);
        }
        else
        {
            qDebug() << "CRC error.";
            readSize = 0;
        }
    }
    else
    {
        qDebug() << "485 readBytes error." << readBytes;
        readSize = 0;
        //Utility::msleep(100); /* Need some delay. */
    }

    //Utility::msleep(100); /* Need some delay. */

    //printDataHex(readVal, readSize);
    return readSize;
}

int ModbusRTUProtocol::checkCRC(char *buf, int size)
{
    ushort crc;
    char  crcHi, crcLo;

    crc   = Utility::crc16(buf, size - 2); /* except CRC */
    crcHi = (crc & 0xFF00) >> 8;
    crcLo = (crc & 0x00FF);

    if ((crcLo != buf[size - 1]) || (crcHi != buf[size - 2]))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

void ModbusRTUProtocol::printDataHex(char *data, int size)
{
    QString hex;
    QDebug debug = qDebug();

    debug << "Received :";
    for (int i = 0; i < size; i++)
        debug << hex.setNum(data[i], 16);
}
