#include "invconfiginfo.h"

#include <QStringList>

InvConfigInfo::InvConfigInfo()
{
}

/*
 * getter
 */
QString InvConfigInfo::getMasterType()
{
    return masterType;
}

QString InvConfigInfo::getProtocolType()
{
    return protocolType;
}

QString InvConfigInfo::getConnectionType()
{
    return connectionType;
}

QString InvConfigInfo::getDeviceName()
{
    return deviceName;
}

QString InvConfigInfo::getBaudrate()
{
    return baudrate;
}

QString InvConfigInfo::getIpAddress()
{
    return ipAddress;
}

QString InvConfigInfo::getPortNumber()
{
    return portNumber;
}

QString InvConfigInfo::getSlaves()
{
    return slaves;
}

QString InvConfigInfo::getOutputCalibration()
{
    return outputCalibration;
}

/*
 * setter
 */
void InvConfigInfo::setMasterType(QString value)
{
    masterType = value;
}

void InvConfigInfo::setProtocolType(QString value)
{
    protocolType = value;
}

void InvConfigInfo::setConnectionType(QString value)
{
    connectionType = value;
}

void InvConfigInfo::setDeviceName(QString value)
{
    deviceName = value;
}

void InvConfigInfo::setBaudrate(QString value)
{
    baudrate = value;
}

void InvConfigInfo::setIpAddress(QString value)
{
    ipAddress = value;
}

void InvConfigInfo::setPortNumber(QString value)
{
    portNumber = value;
}

void InvConfigInfo::setSlaves(QString value)
{
    slaves = value;
}

void InvConfigInfo::setOutputCalibration(QString value)
{
    outputCalibration = value;
}

QString InvConfigInfo::toString()
{
    QStringList temp;

    temp.append(masterType);
    temp.append(protocolType);
    temp.append(connectionType);
    temp.append(deviceName);
    temp.append(baudrate);
    temp.append(ipAddress);
    temp.append(portNumber);
    temp.append(slaves);
    temp.append(outputCalibration);

    return temp.join(",");
}
