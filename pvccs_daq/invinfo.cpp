#include "invinfo.h"

#include <QStringList>

InvInfo::InvInfo()
{
    groupId = "0.0";
    slaveId = "0.0";
    instantPower = "0.0";
    generatedPowerDaily = "0.0";
    generatedPowerTotal = "0.0";
    pvVoltage = "0.0";
    pvCurrent = "0.0";
    pvPower = "0.0";
    tRadiation = "0.0";
    hRadiation = "0.0";
    outTemperature = "0.0";
    moduleTemperature = "0.0";
    status = "0000|0000|0000|0000";
}

/*
 * InvInfo::getter
 */
QString InvInfo::getGroupId()
{
    return groupId;
}

QString InvInfo::getSlaveId()
{
    return slaveId;
}

QString InvInfo::getInstantPower()
{
    return instantPower;
}

QString InvInfo::getGeneratedPowerDaily()
{
    return generatedPowerDaily;
}

QString InvInfo::getGeneratedPowerTotal()
{
    return generatedPowerTotal;
}

QString InvInfo::getPvVoltage()
{
    return pvVoltage;
}

QString InvInfo::getPvCurrent()
{
    return pvCurrent;
}

QString InvInfo::getPvPower()
{
    return pvPower;
}

QString InvInfo::getTRadiation()
{
    return tRadiation;
}

QString InvInfo::getHRadiation()
{
    return hRadiation;
}

QString InvInfo::getOutTemperature()
{
    return outTemperature;
}

QString InvInfo::getModuleTemperature()
{
    return moduleTemperature;
}

QString InvInfo::getStatus()
{
    return status;
}

/*
 * InvInfo::setter
 */
void InvInfo::setGroupId(QString value)
{
    groupId = value;
}

void InvInfo::setSlaveId(QString value)
{
    slaveId = value;
}

void InvInfo::setInstantPower(QString value)
{
    instantPower = value;
}

void InvInfo::setGeneratedPowerDaily(QString value)
{
    generatedPowerDaily = value;
}

void InvInfo::setGeneratedPowerTotal(QString value)
{
    generatedPowerTotal = value;
}

void InvInfo::setPvVoltage(QString value)
{
    pvVoltage = value;
}

void InvInfo::setPvCurrent(QString value)
{
    pvCurrent = value;
}

void InvInfo::setPvPower(QString value)
{
    pvPower = value;
}

void InvInfo::setTRadiation(QString value)
{
    tRadiation = value;
}

void InvInfo::setHRadiation(QString value)
{
    hRadiation = value;
}

void InvInfo::setOutTemperature(QString value)
{
    outTemperature = value;
}

void InvInfo::setModuleTemperature(QString value)
{
    moduleTemperature = value;
}

void InvInfo::setStatus(QString value)
{
    status = value;
}

QString InvInfo::toString()
{
    QStringList temp;

    temp.append(groupId);
    temp.append(slaveId);
    temp.append(instantPower);
    temp.append(generatedPowerDaily);
    temp.append(generatedPowerTotal);
    temp.append(pvVoltage);
    temp.append(pvCurrent);
    temp.append(pvPower);
    temp.append(tRadiation);
    temp.append(hRadiation);
    temp.append(outTemperature);
    temp.append(moduleTemperature);
    temp.append(status);

    return temp.join(",");
}
