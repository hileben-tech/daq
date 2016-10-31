#ifndef SERPORT_H
#define SERPORT_H

#include <termios.h>

#include <QObject>
#include <QByteArray>

class SerPort : public QObject
{
    Q_OBJECT

    int fd;
    struct termios oldtio;

    int timeout;

public:
    explicit SerPort(QObject *parent = 0);
    virtual ~SerPort();

    virtual int openPort(QString portName, long baudRate, int dataBits, int stopBits, int parity);
    virtual int openPort(QString portName, long baudRate);

    virtual int isPortOpen();
    virtual void closePort();

    void setTimeout(int param);

    virtual QByteArray readBytes(int size);
    virtual QByteArray readLine();
    virtual QByteArray readByDeterminer(const char deter);
    virtual QByteArray readByStxEtx(const char stx, const char etx);

    virtual int writeBytes(QByteArray data);

protected:
    virtual int config(long baudRate, int dataBits, int stopBits, int parity);

signals:

public slots:

};

#endif // SERPORT_H
