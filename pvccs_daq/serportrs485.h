#ifndef SERPORTPRS485_H
#define SERPORTPRS485_H

#include <termios.h>

#include "serport.h"
#include <QObject>
#include <QByteArray>

class SerPortRS485
{
    int fd;
    int timeout;
    struct termios oldtio;

public:
    explicit SerPortRS485(QObject *parent = 0);
    ~SerPortRS485();

    int openPort(QString portName, long baudRate, int dataBits, int stopBits, int parity);
    int openPort(QString portName, long baudRate);

    int  isPortOpen();
    void closePort();

    void setTimeout(int param);

    int readBytes(char *readBuf, int size);
    int writeBytes(char *dataBuf, int size);

    int config(long baudRate, int dataBits, int stopBits, int parity);

signals:

public slots:

};

#endif // SERPORTPRS485_H
