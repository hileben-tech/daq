#ifndef INVINFO_H
#define INVINFO_H

#include <QString>

class InvInfo
{
    QString groupId;
    QString slaveId;
    QString instantPower;
    QString generatedPowerDaily;
    QString generatedPowerTotal;
    QString pvVoltage;
    QString pvCurrent;
    QString pvPower;
    QString tRadiation;
    QString hRadiation;
    QString outTemperature;
    QString moduleTemperature;
    QString status;

public:
    InvInfo();

    /*
     * getter
     */
    QString getGroupId();
    QString getSlaveId();
    QString getInstantPower();
    QString getGeneratedPowerDaily();
    QString getGeneratedPowerTotal();
    QString getPvVoltage();
    QString getPvCurrent();
    QString getPvPower();
    QString getTRadiation();
    QString getHRadiation();
    QString getOutTemperature();
    QString getModuleTemperature();
    QString getStatus();

    /*
     * setter
     */
    void setGroupId(QString value);
    void setSlaveId(QString value);
    void setInstantPower(QString value);
    void setGeneratedPowerDaily(QString value);
    void setGeneratedPowerTotal(QString value);
    void setPvVoltage(QString value);
    void setPvCurrent(QString value);
    void setPvPower(QString value);
    void setTRadiation(QString value);
    void setHRadiation(QString value);
    void setOutTemperature(QString value);
    void setModuleTemperature(QString value);
    void setStatus(QString value);

    QString toString();

};

#endif // INVINFO_H
