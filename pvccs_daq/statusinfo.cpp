#include "buildopts.h"
#include "statusinfo.h"
#include <QDebug>
#include <QStringList>

StatusInfo::StatusInfo()
{
    filterPressure = "0";
    pump1Pressure = "0";
    pump2Pressure = "0";
    pump1Ct = "0";
    pump2Ct = "0";
    radiation = "0";
    installedTemperature = "0";
    uninstalledTemperature = "0";
    airTemperature = "0";

    supplyValve = "0";
    waterLevelH = "0";
    waterLevelM = "0";
    waterLevelL = "0";
    saltLevelL = "0";
    indraftValve = "0";
    pump1 = "0";
    pump2 = "0";
    drainValve = "0";
    sector1Valve = "0";
    sector2Valve = "0";
    sector3Valve = "0";
    sector4Valve = "0";
    sector5Valve = "0";
    doorOpen = "0";
    osShutdown = "0";
    waterSpray = "0";
    waterDrain = "0";
    warnWaterLevelL = "0";
    warnSaltLevelL = "0";

    warnFrozen = "0";
    cloudy = "0";
    ioBoard = "0";
    inverter1 = "0";
    inverter2 = "0";
    inverter3 = "0";
    inverter4 = "0";
    webServer = "0";
    filterPressureSensor = "0";
    pump1PressureSensor = "0";
    pump2PressureSensor = "0";
    pump1CtModule = "0";
    pump2CtModule = "0";
#if (BOPTS_FIX_VERSION_INFO == 1)
    ioVersion = "00";
#endif
}

/*
 * getter
 */
QString StatusInfo::getFilterPressure()
{
    return filterPressure;
}

QString StatusInfo::getPump1Pressure()
{
    return pump1Pressure;
}

QString StatusInfo::getPump2Pressure()
{
    return pump2Pressure;
}

QString StatusInfo::getPump1Ct()
{
    return pump1Ct;
}

QString StatusInfo::getPump2Ct()
{
    return pump2Ct;
}

QString StatusInfo::getRadiation()
{
    return radiation;
}

QString StatusInfo::getInstalledTemperature()
{
    return installedTemperature;
}

QString StatusInfo::getUninstalledTemperature()
{
    return uninstalledTemperature;
}

QString StatusInfo::getAirTemperature()
{
    return airTemperature;
}

QString StatusInfo::getSupplyValve()
{
    return supplyValve;
}

QString StatusInfo::getWaterLevelH()
{
    return waterLevelH;
}

QString StatusInfo::getWaterLevelM()
{
    return waterLevelM;
}

QString StatusInfo::getWaterLevelL()
{
    return waterLevelL;
}

QString StatusInfo::getSaltLevelL()
{
    return saltLevelL;
}

QString StatusInfo::getIndraftValve()
{
    return indraftValve;
}

QString StatusInfo::getPump1()
{
    return pump1;
}

QString StatusInfo::getPump2()
{
    return pump2;
}

QString StatusInfo::getDrainValve()
{
    return drainValve;
}

QString StatusInfo::getSector1Valve()
{
    return sector1Valve;
}

QString StatusInfo::getSector2Valve()
{
    return sector2Valve;
}

QString StatusInfo::getSector3Valve()
{
    return sector3Valve;
}

QString StatusInfo::getSector4Valve()
{
    return sector4Valve;
}

QString StatusInfo::getSector5Valve()
{
    return sector5Valve;
}

QString StatusInfo::getDoorOpen()
{
    return doorOpen;
}

QString StatusInfo::getOsShutdown()
{
    return osShutdown;
}

QString StatusInfo::getWaterSpray()
{
    return waterSpray;
}

QString StatusInfo::getWaterDrain()
{
    return waterDrain;
}

QString StatusInfo::getWarnWaterLevelL()
{
    return warnWaterLevelL;
}

QString StatusInfo::getWarnSaltLevelL()
{
    return warnSaltLevelL;
}

QString StatusInfo::getWarnFrozen()
{
    return warnFrozen;
}

QString StatusInfo::getCloudy()
{
    return cloudy;
}

QString StatusInfo::getIoBoard()
{
    return ioBoard;
}

QString StatusInfo::getInverter1()
{
    return inverter1;
}

QString StatusInfo::getInverter2()
{
    return inverter2;
}

QString StatusInfo::getInverter3()
{
    return inverter3;
}

QString StatusInfo::getInverter4()
{
    return inverter4;
}

QString StatusInfo::getWebServer()
{
    return webServer;
}

QString StatusInfo::getFilterPressureSensor()
{
    return filterPressureSensor;
}

QString StatusInfo::getPump1PressureSensor()
{
    return pump1PressureSensor;
}

QString StatusInfo::getPump2PressureSensor()
{
    return pump2PressureSensor;
}

QString StatusInfo::getPump1CtModule()
{
    return pump1CtModule;
}

QString StatusInfo::getPump2CtModule()
{
    return pump2CtModule;
}

#if (BOPTS_FIX_VERSION_INFO == 1)
QString StatusInfo::getIOVersion()
{
    return ioVersion;
}
#endif

/*
 * setter
 */
void StatusInfo::setFilterPressure(QString value)
{
    filterPressure = value;
}

void StatusInfo::setPump1Pressure(QString value)
{
    pump1Pressure = value;
}

void StatusInfo::setPump2Pressure(QString value)
{
    pump2Pressure = value;
}

void StatusInfo::setPump1Ct(QString value)
{
    pump1Ct = value;
}

void StatusInfo::setPump2Ct(QString value)
{
    pump2Ct = value;
}

void StatusInfo::setRadiation(QString value)
{
    radiation = value;
}

void StatusInfo::setInstalledTemperature(QString value)
{
    //qDebug() << "[StatusInfo::setInstalledTemperature]" << value;
    installedTemperature = value;
}

void StatusInfo::setUninstalledTemperature(QString value)
{
    //qDebug() << "[StatusInfo::setUninstalledTemperature]" << value;
    uninstalledTemperature = value;
}

void StatusInfo::setAirTemperature(QString value)
{
    airTemperature = value;
}

void StatusInfo::setSupplyValve(QString value)
{
    supplyValve = value;
}

void StatusInfo::setWaterLevelH(QString value)
{
    waterLevelH = value;
}

void StatusInfo::setWaterLevelM(QString value)
{
    waterLevelM = value;
}

void StatusInfo::setWaterLevelL(QString value)
{
    waterLevelL = value;
}

void StatusInfo::setSaltLevelL(QString value)
{
    saltLevelL = value;
}

void StatusInfo::setIndraftValve(QString value)
{
    indraftValve = value;
}

void StatusInfo::setPump1(QString value)
{
    pump1 = value;
}

void StatusInfo::setPump2(QString value)
{
    pump2 = value;
}

void StatusInfo::setDrainValve(QString value)
{
    drainValve = value;
}

void StatusInfo::setSector1Valve(QString value)
{
    sector1Valve = value;
}

void StatusInfo::setSector2Valve(QString value)
{
    sector2Valve = value;
}

void StatusInfo::setSector3Valve(QString value)
{
    sector3Valve = value;
}

void StatusInfo::setSector4Valve(QString value)
{
    sector4Valve = value;
}

void StatusInfo::setSector5Valve(QString value)
{
    sector5Valve = value;
}

void StatusInfo::setDoorOpen(QString value)
{
    doorOpen = value;
}

void StatusInfo::setOsShutdown(QString value)
{
    osShutdown = value;
}

void StatusInfo::setWaterSpray(QString value)
{
    waterSpray = value;
}

void StatusInfo::setWaterDrain(QString value)
{
    waterDrain = value;
}

void StatusInfo::setWarnWaterLevelL(QString value)
{
    warnWaterLevelL = value;
}

void StatusInfo::setWarnSaltLevelL(QString value)
{
    warnSaltLevelL = value;
}

void StatusInfo::setWarnFrozen(QString value)
{
    warnFrozen = value;
}

void StatusInfo::setCloudy(QString value)
{
    cloudy = value;
}

void StatusInfo::setIoBoard(QString value)
{
    ioBoard = value;
}

void StatusInfo::setInverter1(QString value)
{
    inverter1 = value;
}

void StatusInfo::setInverter2(QString value)
{
    inverter2 = value;
}

void StatusInfo::setInverter3(QString value)
{
    inverter3 = value;
}

void StatusInfo::setInverter4(QString value)
{
    inverter4 = value;
}

void StatusInfo::setWebServer(QString value)
{
    webServer = value;
}

void StatusInfo::setFilterPressureSensor(QString value)
{
    filterPressureSensor = value;
}

void StatusInfo::setPump1PressureSensor(QString value)
{
    pump1PressureSensor = value;
}

void StatusInfo::setPump2PressureSensor(QString value)
{
    pump2PressureSensor = value;
}

void StatusInfo::setPump1CtModule(QString value)
{
    pump1CtModule = value;
}

void StatusInfo::setPump2CtModule(QString value)
{
    pump2CtModule = value;
}

#if (BOPTS_FIX_VERSION_INFO == 1)
void StatusInfo::setIOVersion(QString value)
{
    //qDebug() << "[StatusInfo::setIOVersion]" << value;
    ioVersion = value;
}
#endif

QString StatusInfo::toString()
{
    QStringList temp;

    temp.append(filterPressure);
    temp.append(pump1Pressure);
    temp.append(pump2Pressure);
    temp.append(pump1Ct);
    temp.append(pump2Ct);
    temp.append(radiation);
    temp.append(installedTemperature);
    temp.append(uninstalledTemperature);
    temp.append(airTemperature);

    temp.append(supplyValve);
    temp.append(waterLevelH);
    temp.append(waterLevelM);
    temp.append(waterLevelL);
    temp.append(saltLevelL);
    temp.append(indraftValve);
    temp.append(pump1);
    temp.append(pump2);
    temp.append(drainValve);
    temp.append(sector1Valve);
    temp.append(sector2Valve);
    temp.append(sector3Valve);
    temp.append(sector4Valve);
    temp.append(sector5Valve);
    temp.append(doorOpen);
    temp.append(osShutdown);
    temp.append(waterSpray);
    temp.append(waterDrain);
    temp.append(warnWaterLevelL);
    temp.append(warnSaltLevelL);

    temp.append(warnFrozen);
    temp.append(cloudy);
    temp.append(ioBoard);
    temp.append(inverter1);
    temp.append(inverter2);
    temp.append(inverter3);
    temp.append(inverter4);
    temp.append(webServer);
    temp.append(filterPressureSensor);
    temp.append(pump1PressureSensor);
    temp.append(pump2PressureSensor);
    temp.append(pump1CtModule);
    temp.append(pump2CtModule);
#if (BOPTS_FIX_VERSION_INFO == 1)
    temp.append(ioVersion);
#endif

    return temp.join(",");

}
