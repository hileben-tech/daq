#ifndef OSCONFIGINFO_H
#define OSCONFIGINFO_H

#include <QString>

class OsConfigInfo
{
    QString plantCode;
    QString filterPressureHigh;
    QString pump1PressureLow;
    QString pump1PressureHigh;
    QString pump1CurrentShort;
    QString pump1CurrentOver;
    QString pump2PressureLow;
    QString pump2PressureHigh;
    QString pump2CurrentShort;
    QString pump2CurrentOver;
    QString sprayProfileName;
    QString sprayProfileItems;
    QString sprayManualTime;
    QString pumpNumber;
    QString sectorNumber;
    QString freezeTemperature;
    QString cloudyTemperature;
    QString inverterMasterType;
    QString inverterProtocolType;
    QString inverterConnectionType;
    QString inverterDeviceName;
    QString inverterBaudrate;
    QString inverterIpAddress;
    QString inverterPortNumber;
    QString inverterSlaves;
    QString temperatureCalibration;
    QString solarPowerCalibration;
    QString version;

public:
    OsConfigInfo();

    /*
     * getter
     */
    QString getPlantCode();
    QString getFilterPressureHigh();
    QString getPump1PressureLow();
    QString getPump1PressureHigh();
    QString getPump1CurrentShort();
    QString getPump1CurrentOver();
    QString getPump2PressureLow();
    QString getPump2PressureHigh();
    QString getPump2CurrentShort();
    QString getPump2CurrentOver();
    QString getSprayProfileName();
    QString getSprayProfileItems();
    QString getSprayManualTime();
    QString getPumpNumber();
    QString getSectorNumber();
    QString getFreezeTemperature();
    QString getCloudyTemperature();
    QString getInverterMasterType();
    QString getInverterProtocolType();
    QString getInverterConnectionType();
    QString getInverterDeviceName();
    QString getInverterBaudrate();
    QString getInverterIpAddress();
    QString getInverterPortNumber();
    QString getInverterSlaves();
    QString getTemperatureCalibration();
    QString getSolarPowerCalibration();
    QString getVersion();

    /*
     * setter
     */
    void setPlantCode(QString value);
    void setFilterPressureHigh(QString value);
    void setPump1PressureLow(QString value);
    void setPump1PressureHigh(QString value);
    void setPump1CurrentShort(QString value);
    void setPump1CurrentOver(QString value);
    void setPump2PressureLow(QString value);
    void setPump2PressureHigh(QString value);
    void setPump2CurrentShort(QString value);
    void setPump2CurrentOver(QString value);
    void setSprayProfileName(QString value);
    void setSprayProfileItems(QString value);
    void setSprayManualTime(QString value);
    void setPumpNumber(QString value);
    void setSectorNumber(QString value);
    void setFreezeTemperature(QString value);
    void setCloudyTemperature(QString value);
    void setInverterMasterType(QString value);
    void setInverterProtocolType(QString value);
    void setInverterConnectionType(QString value);
    void setInverterDeviceName(QString value);
    void setInverterBaudrate(QString value);
    void setInverterIpAddress(QString value);
    void setInverterPortNumber(QString value);
    void setInverterSlaves(QString value);
    void setTemperatureCalibration(QString value);
    void setSolarPowerCalibration(QString value);
    void setVersion(QString value);

    QString toString();

};

#endif // OSCONFIGINFO_H
