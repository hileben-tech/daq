#ifndef INVCONFIGINFO_H
#define INVCONFIGINFO_H

#include <QString>

class InvConfigInfo
{
    QString masterType;
    QString protocolType;
    QString connectionType;
    QString deviceName;
    QString baudrate;
    QString ipAddress;
    QString portNumber;
    QString slaves;
    QString outputCalibration;

public:
    InvConfigInfo();

    /*
     * getter
     */
    QString getMasterType();
    QString getProtocolType();
    QString getConnectionType();
    QString getDeviceName();
    QString getBaudrate();
    QString getIpAddress();
    QString getPortNumber();
    QString getSlaves();
    QString getOutputCalibration();

    /*
     * setter
     */
    void setMasterType(QString value);
    void setProtocolType(QString value);
    void setConnectionType(QString value);
    void setDeviceName(QString value);
    void setBaudrate(QString value);
    void setIpAddress(QString value);
    void setPortNumber(QString value);
    void setSlaves(QString value);
    void setOutputCalibration(QString value);

    QString toString();

};

#endif // INVCONFIGINFO_H
