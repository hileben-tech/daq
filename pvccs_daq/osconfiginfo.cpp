#include "osconfiginfo.h"

#include "version.h"
#include <QDebug>
#include <QStringList>

OsConfigInfo::OsConfigInfo()
{
}

/*
 * getter
 */
QString OsConfigInfo::getPlantCode()
{
    return plantCode;
}

QString OsConfigInfo::getFilterPressureHigh()
{
    return filterPressureHigh;
}

QString OsConfigInfo::getPump1PressureLow()
{
    return pump1PressureLow;
}

QString OsConfigInfo::getPump1PressureHigh()
{
    return pump1PressureHigh;
}

QString OsConfigInfo::getPump1CurrentShort()
{
    return pump1CurrentShort;
}

QString OsConfigInfo::getPump1CurrentOver()
{
    return pump1CurrentOver;
}

QString OsConfigInfo::getPump2PressureLow()
{
    return pump2PressureLow;
}

QString OsConfigInfo::getPump2PressureHigh()
{
    return pump2PressureHigh;
}

QString OsConfigInfo::getPump2CurrentShort()
{
    return pump2CurrentShort;
}

QString OsConfigInfo::getPump2CurrentOver()
{
    return pump2CurrentOver;
}

QString OsConfigInfo::getSprayProfileName()
{
    return sprayProfileName;
}

QString OsConfigInfo::getSprayProfileItems()
{
    return sprayProfileItems;
}

QString OsConfigInfo::getSprayManualTime()
{
    return sprayManualTime;
}

QString OsConfigInfo::getPumpNumber()
{
    return pumpNumber;
}

QString OsConfigInfo::getSectorNumber()
{
    return sectorNumber;
}

QString OsConfigInfo::getFreezeTemperature()
{
    return freezeTemperature;
}

QString OsConfigInfo::getCloudyTemperature()
{
    return cloudyTemperature;
}

QString OsConfigInfo::getInverterMasterType()
{
    return inverterMasterType;
}

QString OsConfigInfo::getInverterProtocolType()
{
    return inverterProtocolType;
}

QString OsConfigInfo::getInverterConnectionType()
{
    return inverterConnectionType;
}

QString OsConfigInfo::getInverterDeviceName()
{
    return inverterDeviceName;
}

QString OsConfigInfo::getInverterBaudrate()
{
    return inverterBaudrate;
}

QString OsConfigInfo::getInverterIpAddress()
{
    return inverterIpAddress;
}

QString OsConfigInfo::getInverterPortNumber()
{
    return inverterPortNumber;
}

QString OsConfigInfo::getInverterSlaves()
{
    return inverterSlaves;
}

QString OsConfigInfo::getTemperatureCalibration()
{
    return temperatureCalibration;
}

QString OsConfigInfo::getSolarPowerCalibration()
{
    return solarPowerCalibration;
}

QString OsConfigInfo::getVersion()
{
    version = Version::getVersion();
    return version;
}

/*
 * setter
 */
void OsConfigInfo::setPlantCode(QString value)
{
    plantCode = value;
}

void OsConfigInfo::setFilterPressureHigh(QString value)
{
    filterPressureHigh = value;
}

void OsConfigInfo::setPump1PressureLow(QString value)
{
    pump1PressureLow = value;
}

void OsConfigInfo::setPump1PressureHigh(QString value)
{
    pump1PressureHigh = value;
}

void OsConfigInfo::setPump1CurrentShort(QString value)
{
    pump1CurrentShort = value;
}

void OsConfigInfo::setPump1CurrentOver(QString value)
{
    pump1CurrentOver = value;
}

void OsConfigInfo::setPump2PressureLow(QString value)
{
    pump2PressureLow = value;
}

void OsConfigInfo::setPump2PressureHigh(QString value)
{
    pump2PressureHigh = value;
}

void OsConfigInfo::setPump2CurrentShort(QString value)
{
    pump2CurrentShort = value;
}

void OsConfigInfo::setPump2CurrentOver(QString value)
{
    pump2CurrentOver = value;
}

void OsConfigInfo::setSprayProfileName(QString value)
{
    sprayProfileName = value;
}

void OsConfigInfo::setSprayProfileItems(QString value)
{
    sprayProfileItems = value;
}

void OsConfigInfo::setSprayManualTime(QString value)
{
    sprayManualTime = value;
}

void OsConfigInfo::setPumpNumber(QString value)
{
    pumpNumber = value;
}

void OsConfigInfo::setSectorNumber(QString value)
{
    sectorNumber = value;
}

void OsConfigInfo::setFreezeTemperature(QString value)
{
    freezeTemperature = value;
}

void OsConfigInfo::setCloudyTemperature(QString value)
{
    cloudyTemperature = value;
}

void OsConfigInfo::setInverterMasterType(QString value)
{
    inverterMasterType = value;
}

void OsConfigInfo::setInverterProtocolType(QString value)
{
    inverterProtocolType = value;
}

void OsConfigInfo::setInverterConnectionType(QString value)
{
    inverterConnectionType = value;
}

void OsConfigInfo::setInverterDeviceName(QString value)
{
    inverterDeviceName = value;
}

void OsConfigInfo::setInverterBaudrate(QString value)
{
    inverterBaudrate = value;
}

void OsConfigInfo::setInverterIpAddress(QString value)
{
    inverterIpAddress = value;
}

void OsConfigInfo::setInverterPortNumber(QString value)
{
    inverterPortNumber = value;
}

void OsConfigInfo::setInverterSlaves(QString value)
{
    inverterSlaves = value;
}

void OsConfigInfo::setTemperatureCalibration(QString value)
{
    temperatureCalibration = value;
}

void OsConfigInfo::setSolarPowerCalibration(QString value)
{
    solarPowerCalibration = value;
}

void OsConfigInfo::setVersion(QString value)
{
    version = value;
    //qDebug() << "[OsConfigInfo::setVersion]" << version << value;
}

QString OsConfigInfo::toString()
{
    QStringList temp;

    temp.append(plantCode);
    temp.append(filterPressureHigh);
    temp.append(pump1PressureLow);
    temp.append(pump1PressureHigh);
    temp.append(pump1CurrentShort);
    temp.append(pump1CurrentOver);
    temp.append(pump2PressureLow);
    temp.append(pump2PressureHigh);
    temp.append(pump2CurrentShort);
    temp.append(pump2CurrentOver);
    temp.append(sprayProfileName);
    temp.append(sprayProfileItems);
    temp.append(sprayManualTime);
    temp.append(pumpNumber);
    temp.append(sectorNumber);
    temp.append(freezeTemperature);
    temp.append(cloudyTemperature);
    temp.append(inverterMasterType);
    temp.append(inverterProtocolType);
    temp.append(inverterConnectionType);
    temp.append(inverterDeviceName);
    temp.append(inverterBaudrate);
    temp.append(inverterIpAddress);
    temp.append(inverterPortNumber);
    temp.append(inverterSlaves);
    temp.append(temperatureCalibration);
    temp.append(solarPowerCalibration);
    temp.append(version);

    return temp.join(",");
}
