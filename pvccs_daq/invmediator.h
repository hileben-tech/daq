#ifndef INVMEDIATOR_H
#define INVMEDIATOR_H

#include "buildopts.h"
#include "invconfiginfo.h"
#include "invinterface.h"
#include "invinfo.h"
#include "osconfiginfo.h"

#include <QThread>
#include <QDateTime>
#include <QStringList>
#include <QMutex>

class InvMediator : public QThread
{
    Q_OBJECT

    InvConfigInfo invConfigInfo;

    InvInterface* invInterface;

    QStringList slaves;

    QMutex mutex;

public:
    explicit InvMediator(QObject *parent = 0);
    ~InvMediator();

    void setupInverter();

private:
    InvConfigInfo converterOsConfigInfoToInvConfigInfo(OsConfigInfo osConfigInfo);

protected:
    virtual void run();

signals:
    void readyInvInfo(InvInfo invInfo, QDateTime dateTime);
    void changedInvConfigInfo(InvConfigInfo invConfigInfo);

public slots:

};

#endif // INVMEDIATOR_H
