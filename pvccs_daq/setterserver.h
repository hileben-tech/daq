#ifndef SETTERSERVER_H
#define SETTERSERVER_H

#include "osconfiginfo.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QDateTime>

class SetterServer : public QTcpServer
{
    Q_OBJECT

    QTcpSocket* setter;

public:
    explicit SetterServer(QObject *parent = 0);

signals:
    void readyCommand(QByteArray owner, QByteArray command);

    void connectedSetter(QByteArray packet, QDateTime dateTime, QString status);
    void disconnectedSetter(QByteArray packet, QDateTime dateTime, QString status);
    void receivedPacket(QByteArray packet, QDateTime dateTime, QString status);
    void sentPacket(QByteArray packet, QDateTime dateTime, QString status);

public slots:

private slots:
    void onNewConnection();
    void onDisconnected();
    void onReadyRead();

};

#endif // SETTERSERVER_H
