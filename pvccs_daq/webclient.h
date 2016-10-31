#ifndef WEBCLIENT_H
#define WEBCLIENT_H

#include "buildopts.h"
#include "osconfiginfo.h"
#include "ostime.h"
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
#include "sckcmodbus.h"
#endif
#include <QThread>
#include <QQueue>
#include <QWaitCondition>
#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDateTime>
#include <QTimer>
#include <QWaitCondition>

class WebClient : public QThread
{
    Q_OBJECT

    QQueue<QByteArray> packetQ;

    QWaitCondition packetAdded;
    QMutex mutex;

    QString domain;

    QNetworkAccessManager *nam;
    QNetworkReply* reply;

    QTimer* timeoutTimer;

	QByteArray  currpacket;

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    COsTime* m_OsTime;
#endif

public:
    explicit WebClient(QObject *parent = 0);

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    void sendTimeReqPacket();
#endif

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
   void sendStatusDAQPacket(SckCModbus *sckcModbus);
#endif

private:
    void ask();

    void addPacket(QByteArray packet);
    QByteArray makePacket(QString type, QString cmd, QString param);
    QByteArray makeReqPacket(QString cmd, QString param);
    QByteArray makeResPacket(QString cmd, QString param);

    void removePacketAll();

protected:
    virtual void run();

signals:
    void sentPacket(QByteArray packet, QDateTime dateTime, QString direction);
    void receivedPacket(QByteArray packet, QDateTime dateTime, QString direction);
    void readyPacket(QByteArray packet, QDateTime dateTime);
    void ignoredPacket(QByteArray packet, QDateTime dateTime, QString reson);
    void timeout();

    void readyCommand(QByteArray owner, QByteArray command);
    void readyConfigParameter(QByteArray parameter);

    void insertedStatusInfo(QString timestamp);
    void insertedInvInfo(QString timestamp);

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    void requestTimeUpdate();
    void receivedWebTime(QString timestamp);
#endif

public slots:
    void onFinished(QNetworkReply *reply);
    void onTimeout();

    void onReadyIoParameter(QByteArray parameter);
    void onReadyInvParameter(QByteArray parameter);
    void onReadyConfigParameter(QByteArray parameter);

private slots:

};

#endif // WEBCLIENT_H
