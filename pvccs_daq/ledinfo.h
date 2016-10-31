#ifndef LEDINFO_H
#define LEDINFO_H

#include <QString>

class LedInfo
{
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

public:
    enum {
        LED_OFF = 0,
        LED_GREEN,
        LED_ORANGE,
        LED_RED
    };

    LedInfo();

    /*
     * getter
     */
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

    /*
     * setter
     */
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

    QString toString();

};

#endif // LEDINFO_H
