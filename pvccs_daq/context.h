#ifndef CONTEXT_H
#define CONTEXT_H

#include "osconfiginfo.h"

#include <QObject>

class Context : public QObject
{
    Q_OBJECT

    static Context* const instance;

    OsConfigInfo osConfigInfo;

    bool useInverterOnly;
    bool trackInverter;
    bool testOs;
    bool useInverter;

    explicit Context(QObject *parent = 0);

public:
    static Context* getInstance();

    OsConfigInfo getOsConfigInfo();
    void setOsConfigInfo(OsConfigInfo param);

    bool getUseInverterOnly();
    void setUseInverterOnly(bool param);
    bool getTrackInverter();
    void setTrackInverter(bool param);
    bool getTestOs();
    void setTestOs(bool param);
    bool getUseInverter();
    void setUseInverter(bool param);

private:
    void readSettings();
    void writeSettings();

signals:

public slots:

};

#endif // CONTEXT_H
