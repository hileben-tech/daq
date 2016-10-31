#include "invserprotocol.h"

InvSerProtocol::InvSerProtocol(QObject *parent) :
    SerPort(parent)
{
}

//virtual
int InvSerProtocol::readInstantPower(int slaveId, double& var)
{
    return 0;
}

//virtual
int InvSerProtocol::readGeneratedPowerDaily(int slaveId, double& var)
{
    return 0;
}

//virtual
int InvSerProtocol::readGeneratedPowerTotal(int slaveId, double& var)
{
    return 0;
}

//virtual
int InvSerProtocol::readPvVoltage(int slaveId, double& var)
{
    return 0;
}

//virtual
int InvSerProtocol::readPvCurrent(int slaveId, double& var)
{
    return 0;
}

//virtual
int InvSerProtocol::readPvPower(int slaveId, double& var)
{
    return 0;
}

//virtual
int InvSerProtocol::readTRadiation(int slaveId, double& var)
{
    return 0;
}

//virtual
int InvSerProtocol::readHRadiation(int slaveId, double& var)
{
    return 0;
}

//virtual
int InvSerProtocol::readOutTemperature(int slaveId, double& var)
{
    return 0;
}

//virtual
int InvSerProtocol::readModuleTemperature(int slaveId, double& var)
{
    return 0;
}

//virtual
int InvSerProtocol::readStatus(int slaveId, QByteArray& var)
{
    return 0;
}

//virtual
bool InvSerProtocol::isOpen()
{
    return isPortOpen();
}

//virtual
void InvSerProtocol::closeProtocol()
{
    closePort();
}

//virtual
int InvSerProtocol::openProtocol(QString portName, long baudRate, int dataBits, int stopBits, int parity)
{
    return openPort(portName, baudRate, dataBits, stopBits, parity);
}

//virtual
int InvSerProtocol::openProtocol(QString portName, long baudRate)
{
    return openPort(portName, baudRate);
}

//virtual
QByteArray InvSerProtocol::deliverMessage(QByteArray param)
{
    return "";
}

//virtual
QByteArray InvSerProtocol::checkSum(QByteArray param)
{
    return "";
}

//virtual
bool InvSerProtocol::checkFrameValidity(QByteArray param)
{
    return false;
}
