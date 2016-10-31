#include "serport.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

SerPort::SerPort(QObject *parent) :
    QObject(parent)
{
    fd = -1;
    timeout = 0;
}

//virtual
SerPort::~SerPort()
{
    if (isPortOpen()) {
        closePort();
    }
}

//virtual
int SerPort::openPort(QString portName, long baudRate, int dataBits, int stopBits, int parity)
{
    fd = open(portName.toLocal8Bit().data(), (O_RDWR | O_NOCTTY));

    if (fd < 0) {
        return (-1);
    }

    if (config(baudRate, dataBits, stopBits, parity) < 0) {
        return (-1);
    }

    return 0;
}

//virtual
int SerPort::openPort(QString portName, long baudRate)
{
    return openPort(portName, baudRate, 8, 1, 0);
}

//virtual
int SerPort::isPortOpen()
{
    return ((fd != -1) ? true : false);
}

//virtual
void SerPort::closePort()
{
    close(fd);

    tcsetattr(fd, TCSANOW, &oldtio);

    fd = -1;
}

void SerPort::setTimeout(int param)
{
    timeout = param;
}

//virtual
QByteArray SerPort::readBytes(int size)
{
    QByteArray buffer;

    for (;;) {
        char ch;

        int len = read(fd, &ch, 1);

        if (len < 0) {
            return "@error";
        }

        if (len == 0) {
            return "@timeout";
        }

        buffer.append(ch);

        if (buffer.size() == size) {
            break;
        }
    }

    return buffer;
}

//virtual
QByteArray SerPort::readLine()
{
    QByteArray buffer;

    for (;;) {
        char ch;

        int len = read(fd, &ch, 1);

        if (len < 0) {
            return "@error";
        }

        if (len == 0) {
            return "@timeout";
        }

        buffer.append(ch);

        if (ch == '\n') {
            break;
        }
    }

    return buffer;
}

//virtual
QByteArray SerPort::readByDeterminer(const char deter)
{
    QByteArray buffer;

    for (;;) {
        char ch;

        int len = read(fd, &ch, 1);

        if (len < 0) {
            return "@error";
        }

        if (len == 0) {
            return "@timeout";
        }

        if (ch == deter || ch == '\n') {
            break;
        }

        buffer.append(ch);
    }

    return buffer;
}

//virtual
QByteArray SerPort::readByStxEtx(const char stx, const char etx)
{
    QByteArray buffer;
    bool isPreserving = false;

    for (;;) 
	{
        char ch;
        int len = read(fd, &ch, 1);

        if (len < 0) 
		{
            return "@error";
        }

        if (len == 0) 
		{
            return "@timeout";
        }

        if (isPreserving == false) 
		{
            if (ch == stx)
			{
                isPreserving = true;

                buffer.clear();

                buffer.append(ch);
            }
        }
        else 
		{
            if (ch == etx) 
			{
                isPreserving = false;

                buffer.append(ch);

                break;
            }
            else 
			{
                buffer.append(ch);
            }
        }
    }

    return buffer;
}

//virtual
int SerPort::writeBytes(QByteArray data)
{
    return write(fd, data.data(), strlen(data));
}

//virtual
int SerPort::config(long baudRate, int dataBits, int stopBits, int parity)
{
    struct termios newtio;
    int speed;

    if (!isPortOpen()) {
        return (-1);
    }

    tcgetattr(fd, &oldtio);

    bzero(&newtio, sizeof(newtio));

    newtio.c_cflag = (CLOCAL | CREAD);
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;

    /*
     * baudRate
     */
    switch (baudRate) {
    case 300:
        speed = B300;
        break;

    case 600:
        speed = B600;
        break;

    case 1200:
        speed = B1200;
        break;

    case 1800:
        speed = B1800;
        break;

    case 2400:
        speed = B2400;
        break;

    case 4800:
        speed = B4800;
        break;

    case 9600:
        speed = B9600;
        break;

    case 19200:
        speed = B19200;
        break;

    case 38400:
        speed = B38400;
        break;

    case 57600:
        speed = B57600;
        break;

    case 115200:
        speed = B115200;
        break;

    case 230400:
        speed = B230400;
        break;

    case 460800:
        speed = B460800;
        break;

    default:
        return (-1);
    }

    if (cfsetispeed(&newtio, (speed_t) speed) < 0) {
        return (-1);
    }

    if (cfsetospeed(&newtio, (speed_t) speed) < 0) {
        return (-1);
    }

    /*
     * dataBits
     */
    switch (dataBits)
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;

    case 8:
        newtio.c_cflag |= CS8;
        break;

    default:
        return (-1);
    }

    /*
     * stopBits
     */
    switch (stopBits)
    {
    case 1:
        newtio.c_cflag &= ~CSTOPB;
        break;

    case 2:
        newtio.c_cflag |= CSTOPB;
        break;

    default:
        return (-1);
    }

    /*
     * parity
     */
    switch (parity)
    {
    case 0:
        newtio.c_iflag &= ~INPCK;
        newtio.c_cflag &= ~PARENB;
        newtio.c_cflag &= ~PARODD;
        break;

    case 1:
        newtio.c_iflag |= INPCK;
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        break;

    case 2:
        newtio.c_iflag |= INPCK;
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;

    default:
        return (-1);
    }

    newtio.c_cc[VMIN]  = 0;
    newtio.c_cc[VTIME] = (timeout * 10);

    tcflush(fd, TCIFLUSH);

    if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
        return (-1);
    }

    return 0;
}
