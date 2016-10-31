/******************************************************************************
 * Filename    : sckcmodbus.cpp
 * Description : Class for SCK-C-MODBUS Solaracheck communication module
 * Create Date : 2016.06.24
 * Writter     : Moon, Sung Won
 *****************************************************************************/
#include "sckcmodbus.h"
#include "utility.h"
#include "modbusrtuprotocol.h"
#include <QDebug>
#include <QStringList>

SckCModbus::SckCModbus(QObject *parent) :
    QObject(parent)
{
    modbus = new ModbusRTUProtocol;

    connected = false;
}

//virtual
SckCModbus::~SckCModbus()
{
    if (modbus)
    {
        disconnect();
        delete modbus;
        modbus = NULL;
    }
}

int SckCModbus::connect(QString portName, int baudrate)
{
    int ret = -1;

    if (modbus)
    {
        ret = modbus->openPort(portName, baudrate);
        if (ret == 0)
        {
            for (int ch = 0; ch < 8; ch++)
            {
                channel[ch].connected = false;
            }
            connected = true;
        }
    }

    return ret;
}

void SckCModbus::disconnect()
{
    if (connected == true)
    {
        modbus->closePort();
        connected = false;
    }
}

bool SckCModbus::isConnected()
{
    return connected;
}

int SckCModbus::setSlave(int slaveAddr)
{
    if (isConnected() == false)
    {
        return -1;
    }

    if (slaveAddr <= 0)
    {
        qDebug() << "Invalid slave address.";
        return -1;
    }

    slaveAddress = slaveAddr;
    return 0;
}

void SckCModbus::setModuleName(int ch, QString moduleName)
{
    mutexData.lock();

    if (ch == 0)
        commModuleName = moduleName;
    else
        channel[ch - 1].moduleName = moduleName;;

    mutexData.unlock();
}

void SckCModbus::setSerialNum(int ch, QString serialNum)
{
    mutexData.lock();

    if (ch == 0)
        commSerialNum = serialNum;
    else
        channel[ch - 1].serialNum = serialNum;;

    mutexData.unlock();
}

void SckCModbus::setCurrent(int ch, QString curr)
{
    mutexData.lock();
    channel[ch - 1].current = curr;
    mutexData.unlock();
    //qDebug() << "[setCurrent]" << portT[ch - 1].current << curr;
}

void SckCModbus::setVoltage(int ch, QString volt)
{
    mutexData.lock();
    channel[ch - 1].voltage = volt;
    mutexData.unlock();
}

int SckCModbus::checkChConnection()
{
    int  connected = 0;
    int  readSize, startAddr;
    char readBuf[4];

#if 1
    for (int ch = 1; ch <= 8; ch++)
    {
        startAddr = REG_SCKCM_CH_DATA_START + (((ch - 1) * REG_SCKCM_CH_DATA_OFFSET) + REG_SCKCM_CH_DATA_STATUS_OFFSET);

        readSize = modbus->readHoldingRegs(slaveAddress, startAddr, REG_SCKCM_CH_DATA_STATUS_CNT, readBuf);

        if (readSize >= (REG_SCKCM_CH_DATA_STATUS_CNT * 2))
        {
            mutexData.lock();
            if (!(readBuf[1] & 0x20))
            {
                channel[ch - 1].connected = true;
                connected++;
                qDebug() << "[SckCModbus::checkPortsConnection]" << ch  << channel[ch - 1].connected;
            }
            else
            {
                channel[ch - 1].connected = false;
            }
            mutexData.unlock();

        }
    }

#else
    /* For test */
    channel[7 - 1].connected = true;
    connected++;

#endif

    return connected;
}

bool SckCModbus::isChConnected(int ch)
{
    if (ch <= 0 || ch > 8)
    {
        return false;
    }

    return channel[ch - 1].connected;
}

QString SckCModbus::getChannelStatus()
{
    QString chStatus;

    mutexData.lock();
    for (int ch = 1; ch <= 8; ch++)
    {
        if (isChConnected(ch) == true)
            chStatus.append("1");
        else
            chStatus.append("0");

        chStatus.append("|");
    }
    //chStatus = channelStatus;
    mutexData.unlock();

    return chStatus;
}

/* ch : 0 ~ 8 */
QString SckCModbus::getSerialNum(int ch)
{
    QString serialNum;

    mutexData.lock();

    if (ch == 0)
        serialNum = commSerialNum;
    else
        serialNum =  channel[ch - 1].serialNum;

    mutexData.unlock();

    return serialNum;
}

QString SckCModbus::getModuleName(int ch)
{
    QString moduleName;

    mutexData.lock();

    if (ch == 0)
        moduleName = commModuleName;
    else
        moduleName =  channel[ch - 1].moduleName;

    mutexData.unlock();

    return moduleName;
}

/* ch : 1 ~ 8 */
QString SckCModbus::getCurrent(int ch)
{
    QString curr;

    mutexData.lock();
    curr = channel[ch - 1].current;
    mutexData.unlock();

    qDebug() << "[getCurrent]" << curr;
    return curr;
}

QString SckCModbus::getVoltage(int ch)
{
    QString volt;

    mutexData.lock();
    volt = channel[ch - 1].voltage;
    mutexData.unlock();

    return volt;
}

/******************************************************************************
 * Port0 = SCK-C-MODBUS, Port1~Port8:module connected to T1~T8
 * Big-Endian (MSB received first)
 *****************************************************************************/
QString SckCModbus::readStatus(int ch)
{
    QString status;
    int  readSize, startAddr;
    char readBuf[4], swapBuf[4];

    if (isConnected() == false)
    {
        return NULL;
    }

    startAddr = REG_SCKCM_CH_DATA_START + (((ch - 1) * REG_SCKCM_CH_DATA_OFFSET) + REG_SCKCM_CH_DATA_STATUS_OFFSET);

    readSize = modbus->readHoldingRegs(slaveAddress, startAddr, REG_SCKCM_CH_DATA_STATUS_CNT, readBuf);

    if (readSize >= (REG_SCKCM_CH_DATA_STATUS_CNT * 2))
    {
        swapByteOrder(readBuf, swapBuf, readSize);
        status.append(QString::number(*((short *)swapBuf)));
    }

    //printDataHex("[Status]", (char *)readBuf, readSize);
    return status;
}

QString SckCModbus::readSerialNumber(int ch)
{
    QString serialNum;
    int  readSize, startAddr;
    char readBuf[32], swapBuf[32];

    if ((isConnected() == false) || (ch < 0) || (ch > 8))
    {
        return NULL;
    }

    if (ch == 0)    /* SCK-C-MODBUS */
        startAddr = REG_SCKCM_SERIAL_NUM;
    else
        startAddr = (ch * REG_SCKCM_CH_MI_START) + REG_SCKCM_CH_MI_SERIAL_NUM_OFFSET;

    readSize = modbus->readHoldingRegs(slaveAddress, startAddr, REG_SCKCM_CH_MI_SERIAL_NUM_CNT, readBuf);
    if (readSize >= (REG_SCKCM_CH_MI_SERIAL_NUM_CNT * 2))
    {
        swapByteOrder(readBuf, swapBuf, readSize);
        serialNum.append(QString::number(*(quint64 *)swapBuf));

    }
    setSerialNum(ch, serialNum);

    //qDebug() << "[Serial Number]" << ch << serialNum;
    //printDataHex("[Serial Number]", (char *)readBuf, readSize);
    //printDataHex("[Serial Number]", (char *)swapBuf, readSize);
    return serialNum;
}

QString SckCModbus::readModuleName(int ch)
{
    QString moduleName;
    int  readSize, startAddr;
    char readBuf[40];

    if ((isConnected() == false) || (ch < 0) || (ch > 8))
    {
        qDebug() << "[readModuleName] ************ error : " << isConnected() << ch;
        return NULL;
    }

    if (ch == 0)
        startAddr = REG_SCKCM_MODULE_NAME;
    else
        startAddr = (ch * REG_SCKCM_CH_MI_START) + REG_SCKCM_CH_MI_MODULE_NAME_OFFSET;

    readSize = modbus->readHoldingRegs(slaveAddress, startAddr, REG_SCKCM_CH_MI_MODULE_NAME_CNT, readBuf);
    if (readSize >= (REG_SCKCM_CH_MI_MODULE_NAME_CNT * 2))
    {
        for (int i = 0; i < readSize; i += 2)
        {
            if (readBuf[i + 1] != 0)
            {
                moduleName.append(QChar(readBuf[i + 1]));
                moduleName.append(QChar(readBuf[i]));
            }
        }
    }
    setModuleName(ch, moduleName);

    //qDebug() << "[ModuleName]" << moduleName;
    //printDataHex("", (char *)readBuf, 32);
    return moduleName;
}

int SckCModbus::readChInfo(int ch)
{
    if ((isChConnected(ch) == false) || (ch < 0) || (ch > 8))
    {
        return -1;
    }

    if (readSerialNumber(ch).isNull() == false)
    {
        if (readModuleName(ch).isNull() == false)
        {
            return 0;
        }
    }

    return -1;
}

QString SckCModbus::readCurrent(int ch)
{
    QString currentStr = "-|-|-|-|-|-|-|-";
    char readBuf[32], swapBuf[32];
    int readSize, startAddr;
    short *readBuf16;

    /* ch 0 is not acceptable. */
    if ((isConnected() == false) || (ch <= 0) || (ch > 8))
    {
        return NULL;
    }

    startAddr = REG_SCKCM_CH_DATA_START + (((ch - 1) * REG_SCKCM_CH_DATA_OFFSET) + REG_SCKCM_CH_DATA_CURRENT_OFFSET);

    int loopCnt = 20;
    while (loopCnt > 0)
    {
        readSize = modbus->readHoldingRegs(slaveAddress, startAddr, REG_SCKCM_CH_DATA_CURRENT_CNT, readBuf);
        if (readSize >= (REG_SCKCM_CH_DATA_CURRENT_CNT * 2))
        {
            swapByteOrder(readBuf, swapBuf, readSize);
            readBuf16 = (short *)swapBuf;
            readSize /= 2;

            currentStr.clear();
            for (int i = 0; i < readSize; i++)
            {
                currentStr.append(QString::number(*readBuf16++));
                if (i < (readSize - 1))
                    currentStr.append('|');
            }
            break;
        }
        else
        {
            loopCnt--;
            qDebug() << "[Current] error " << readSize;
        }
    }

    setCurrent(ch, currentStr);

    qDebug() << "[Current]" << currentStr;
    //printDataHex("[Current]", (char *)readBuf, 16);

    return currentStr;
}

QString SckCModbus::readVoltage(int ch)
{
    QString voltStr = "-";
    char readBuf[4], swapBuf[4];
    int  readSize, startAddr;

    /* ch 0 is not acceptable. */
    if ((isConnected() == false) || (ch <= 0) || (ch > 8))
    {
        return NULL;
    }

    startAddr = REG_SCKCM_CH_DATA_START + (((ch - 1) * REG_SCKCM_CH_DATA_OFFSET) + REG_SCKCM_CH_DATA_VOLT_OFFSET);

    int loopCnt = 20;
    while (loopCnt > 0)
    {
        readSize = modbus->readHoldingRegs(slaveAddress, startAddr, REG_SCKCM_CH_DATA_VOLT_CNT, readBuf);
        if (readSize >= (REG_SCKCM_CH_DATA_VOLT_CNT *2))
        {
            swapByteOrder(readBuf, swapBuf, readSize);
            voltStr.clear();
            voltStr.append(QString::number(*(short *)swapBuf));
            break;
        }
        else
        {
            loopCnt--;
            qDebug() << "[readVoltage] error ";
        }
    }

    setVoltage(ch, voltStr);

    qDebug() << "[Voltage]" << voltStr;
    //printDataHex("[Voltage]", readBuf, 2);
    return voltStr;
}

QString SckCModbus::readTemperature(int ch)
{
    QString tempStr = "-";
    char readBuf[4], swapBuf[4];
    int  readSize, startAddr;

    /* ch 0 is not acceptable. */
    if ((isConnected() == false) || (ch <= 0) || (ch > 8))
    {
        return NULL;
    }

    startAddr = REG_SCKCM_CH_DATA_START + (((ch - 1) * REG_SCKCM_CH_DATA_OFFSET) + REG_SCKCM_CH_DATA_TEMP_OFFSET);

    readSize = modbus->readHoldingRegs(slaveAddress, startAddr, REG_SCKCM_CH_DATA_TEMP_CNT, readBuf);
    if (readSize >= (REG_SCKCM_CH_DATA_TEMP_CNT * 2))
    {
        swapByteOrder(readBuf, swapBuf, readSize);
        tempStr.clear();
        tempStr.append(QString::number(*(short *)swapBuf));
    }

    //qDebug() << "[Temperature]" << tempStr;
    //printDataHex("[Temperature]", readBuf, 2);

    return tempStr;
}

void SckCModbus::swapByteOrder(char *srcBuf, char *destBuf, int bytes)
{
    for (int i = 0; i < bytes; i += 2)
    {
        destBuf[i]     = srcBuf[i + 1];
        destBuf[i + 1] = srcBuf[i];
    }
}


/******************************************************************************
 *  For debuggning
 *****************************************************************************/
void SckCModbus::printDataHex(QString msg, char *data, int size)
{
    QString hex;
    QDebug debug = qDebug();

    if (msg.isEmpty() == false)
        qDebug() << msg;

    debug << "Hex:";
    for (int i = 0; i < size; i++)
        debug << hex.setNum(data[i], 16);
}
