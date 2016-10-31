#ifndef STATUSINFO_H
#define STATUSINFO_H
#include "buildopts.h"
#include <QString>

class StatusInfo
{
public:
    /*
     * value
     */
    QString filterPressure;
    QString pump1Pressure;
    QString pump2Pressure;
    QString pump1Ct;
    QString pump2Ct;
    QString radiation;
    QString installedTemperature;
    QString uninstalledTemperature;
    QString airTemperature;

    /*
     * status
     */
    QString supplyValve;
    QString waterLevelH;
    QString waterLevelM;
    QString waterLevelL;
    QString saltLevelL;
    QString indraftValve;
    QString pump1;
    QString pump2;
    QString drainValve;
    QString sector1Valve;
    QString sector2Valve;
    QString sector3Valve;
    QString sector4Valve;
    QString sector5Valve;
    QString doorOpen;
    QString osShutdown;
    QString waterSpray;
    QString waterDrain;
    QString warnWaterLevelL;
    QString warnSaltLevelL;

    QString warnFrozen;
    QString cloudy;
    QString ioBoard;
    QString inverter1;
    QString inverter2;
    QString inverter3;
    QString inverter4;
    QString webServer;
    QString filterPressureSensor;
    QString pump1PressureSensor;
    QString pump2PressureSensor;
    QString pump1CtModule;
    QString pump2CtModule;

#if (BOPTS_FIX_VERSION_INFO == 1)
    QString ioVersion;
#endif

public:
    StatusInfo();

    /*
     * getter
     */
    QString getFilterPressure();
    QString getPump1Pressure();
    QString getPump2Pressure();
    QString getPump1Ct();
    QString getPump2Ct();
    QString getRadiation();
    QString getInstalledTemperature();
    QString getUninstalledTemperature();
    QString getAirTemperature();

    QString getSupplyValve();
    QString getWaterLevelH();
    QString getWaterLevelM();
    QString getWaterLevelL();
    QString getSaltLevelL();
    QString getIndraftValve();
    QString getPump1();
    QString getPump2();
    QString getDrainValve();
    QString getSector1Valve();
    QString getSector2Valve();
    QString getSector3Valve();
    QString getSector4Valve();
    QString getSector5Valve();
    QString getDoorOpen();
    QString getOsShutdown();
    QString getWaterSpray();
    QString getWaterDrain();
    QString getWarnWaterLevelL();
    QString getWarnSaltLevelL();

    QString getWarnFrozen();
    QString getCloudy();
    QString getIoBoard();
    QString getInverter1();
    QString getInverter2();
    QString getInverter3();
    QString getInverter4();
    QString getWebServer();
    QString getFilterPressureSensor();
    QString getPump1PressureSensor();
    QString getPump2PressureSensor();
    QString getPump1CtModule();
    QString getPump2CtModule();

#if (BOPTS_FIX_VERSION_INFO == 1)
    QString getIOVersion();
#endif

    /*
     * setter
     */
    void setFilterPressure(QString value);
    void setPump1Pressure(QString value);
    void setPump2Pressure(QString value);
    void setPump1Ct(QString value);
    void setPump2Ct(QString value);
    void setRadiation(QString value);
    void setInstalledTemperature(QString value);
    void setUninstalledTemperature(QString value);
    void setAirTemperature(QString value);

    void setSupplyValve(QString value);
    void setWaterLevelH(QString value);
    void setWaterLevelM(QString value);
    void setWaterLevelL(QString value);
    void setSaltLevelL(QString value);
    void setIndraftValve(QString value);
    void setPump1(QString value);
    void setPump2(QString value);
    void setDrainValve(QString value);
    void setSector1Valve(QString value);
    void setSector2Valve(QString value);
    void setSector3Valve(QString value);
    void setSector4Valve(QString value);
    void setSector5Valve(QString value);
    void setDoorOpen(QString value);
    void setOsShutdown(QString value);
    void setWaterSpray(QString value);
    void setWaterDrain(QString value);
    void setWarnWaterLevelL(QString value);
    void setWarnSaltLevelL(QString value);

    void setWarnFrozen(QString value);
    void setCloudy(QString value);
    void setIoBoard(QString value);
    void setInverter1(QString value);
    void setInverter2(QString value);
    void setInverter3(QString value);
    void setInverter4(QString value);
    void setWebServer(QString value);
    void setFilterPressureSensor(QString value);
    void setPump1PressureSensor(QString value);
    void setPump2PressureSensor(QString value);
    void setPump1CtModule(QString value);
    void setPump2CtModule(QString value);
#if (BOPTS_FIX_VERSION_INFO == 1)
    void setIOVersion(QString value);
#endif

    QString toString();

};

#endif // STATUSINFO_H
