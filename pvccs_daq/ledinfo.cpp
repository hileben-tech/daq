#include "ledinfo.h"

#include <QStringList>

LedInfo::LedInfo()
{
}

/*
 * getter
 */
QString LedInfo::getWarnFrozen()
{
    return warnFrozen;
}

QString LedInfo::getCloudy()
{
    return cloudy;
}

QString LedInfo::getIoBoard()
{
    return ioBoard;
}

QString LedInfo::getInverter1()
{
    return inverter1;
}

QString LedInfo::getInverter2()
{
    return inverter2;
}

QString LedInfo::getInverter3()
{
    return inverter3;
}

QString LedInfo::getInverter4()
{
    return inverter4;
}

QString LedInfo::getWebServer()
{
    return webServer;
}

QString LedInfo::getFilterPressureSensor()
{
    return filterPressureSensor;
}

QString LedInfo::getPump1PressureSensor()
{
    return pump1PressureSensor;
}

QString LedInfo::getPump2PressureSensor()
{
    return pump2PressureSensor;
}

QString LedInfo::getPump1CtModule()
{
    return pump1CtModule;
}

QString LedInfo::getPump2CtModule()
{
    return pump2CtModule;
}

/*
 * setter
 */
void LedInfo::setWarnFrozen(QString value)
{
    warnFrozen = value;
}

void LedInfo::setCloudy(QString value)
{
    cloudy = value;
}

void LedInfo::setIoBoard(QString value)
{
    ioBoard = value;
}

void LedInfo::setInverter1(QString value)
{
    inverter1 = value;
}

void LedInfo::setInverter2(QString value)
{
    inverter2 = value;
}

void LedInfo::setInverter3(QString value)
{
    inverter3 = value;
}

void LedInfo::setInverter4(QString value)
{
    inverter4 = value;
}

void LedInfo::setWebServer(QString value)
{
    webServer = value;
}

void LedInfo::setFilterPressureSensor(QString value)
{
    filterPressureSensor = value;
}

void LedInfo::setPump1PressureSensor(QString value)
{
    pump1PressureSensor = value;
}

void LedInfo::setPump2PressureSensor(QString value)
{
    pump2PressureSensor = value;
}

void LedInfo::setPump1CtModule(QString value)
{
    pump1CtModule = value;
}

void LedInfo::setPump2CtModule(QString value)
{
    pump2CtModule = value;
}

QString LedInfo::toString()
{
    QStringList temp;

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

    return temp.join(",");
}
