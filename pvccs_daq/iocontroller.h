#ifndef IOCONTROLLER_H
#define IOCONTROLLER_H

#include "serport.h"
#include "statusinfo.h"

#include <QMetaType>
#include <QRunnable>
#include <QQueue>
#include <QWaitCondition>
#include <QMutex>
#include <QDateTime>
#include <QByteArray>
#include <QTimer>

class IOController : public SerPort, public QRunnable
{
    Q_OBJECT

    QQueue<QByteArray> packetQ;

    QWaitCondition packetAdded;
    QMutex mutex;

public:
    explicit IOController(QObject *parent = 0);

    void emergencyStop(QString owner);
    void sprayWaterAutomatically(QString owner, QString param);
    void sprayWaterManually(QString owner);
    void drainWater(QString owner);
    void releaseAlarm(QString owner);
    void getStatus(QString owner);
    void changeLedStatusTo(QString owner, QString param);
    void setupIo(QString owner);

protected:
    virtual void run();

private:
    QByteArray ask(QByteArray packet);

    StatusInfo makeStatusInfoFromParameter(QByteArray param);

    void addPacket(QByteArray packet);
    bool isValidPacket(QByteArray packet);
    void removePacketAll();
    bool processPacket(QByteArray packet);

signals:
    void sentPacket(QByteArray packet, QDateTime dateTime, QString direction);
    void receivedPacket(QByteArray packet, QDateTime dateTime, QString direction);
    void ignoredPacket(QByteArray packet, QDateTime dateTime, QString reson);

    void readyStatusInfo(StatusInfo statusInfo);
    void timeout();

public slots:

};

#endif // IOCONTROLLER_H
