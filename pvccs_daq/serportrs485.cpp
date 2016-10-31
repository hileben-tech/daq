#include "serportrs485.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <QDebug>


SerPortRS485::SerPortRS485(QObject *parent)
{
    fd = -1;
    timeout = 1000;
}

SerPortRS485::~SerPortRS485()
{
    if (isPortOpen())
    {
        closePort();
    }
}

int SerPortRS485::openPort(QString portName, long baudRate, int dataBits, int stopBits, int parity)
{
    struct serial_rs485 rs485Conf;

    fd = open(portName.toLatin1().constData(), O_RDWR | O_NONBLOCK);
    if (fd < 0)
    {
        goto failed;
    }

    if (config(baudRate, dataBits, stopBits, parity) < 0)
    {
        goto failed;
    }

    rs485Conf.flags = SER_RS485_ENABLED;// | SER_RS485_USE_GPIO;
    //rs485.delay_rts_before_send = 0;
    //rs485.delay_rts_after_send = 0;
    //rs485.gpio_pin = 0;
    if (ioctl(fd, TIOCSRS485, &rs485Conf) < 0)
    {
        qDebug() << "[ModbusRTUProtocol::openPort] ioctl failed.";
        goto failed;
    }

    return 0;

failed :
    if (fd > 0)
    {
        close(fd);
    }
    fd = -1;
    return -1;
}

int SerPortRS485::openPort(QString portName, long baudRate)
{
    return openPort(portName, baudRate, 8, 1, 0);
}

int SerPortRS485::isPortOpen()
{
    return ((fd > 0) ? true : false);
}

void SerPortRS485::closePort()
{
    close(fd);
    fd = -1;
}

void SerPortRS485::setTimeout(int cnt)
{
    timeout = cnt;
}

int SerPortRS485::readBytes(char *readBuf, int size)
{
    int readLen, readTotal = 0;
    char buf[size + 1];
    int  pollState;
    struct pollfd pollEvents;

#if 1
    while (1)
    {
        readLen = read(fd, buf, sizeof(buf));
        if ((readLen == 1) && (buf[0] == 0))
        {
            //qDebug() << "[readBytes] EOF" << readLen << ":" << buf[0];
            break; /* EOF */
        }

        if (readLen > 0)
        {
            memcpy(&readBuf[readTotal], buf, readLen);
            readTotal += readLen;
            //qDebug() << "[readBytes] OK" << readLen;
            //qDebug() << "[readBytes] OK" << readLen;

            if (readTotal == size)
            {
                break;
            }
        }
        else if (readLen < 0)
        {
            qDebug() << "[readBytes]" << readLen;
            break;
        }
        else
        {
            qDebug() << "[readBytes]" << readLen;

        }
    }

#else
    pollEvents.fd      = fd;
    pollEvents.events  = POLLIN | POLLERR | POLLHUP;
    pollEvents.revents = 0;

    while (1)
    {
        pollState = poll(&pollEvents,       /* events sturcture */
                                1,          /* pollfd number */
                                timeout);   /* timeout in ms */

        if (pollState > 0)
        {
            if (pollEvents.revents & POLLIN)    /* data received */
            {
                readLen = read(fd, buf, sizeof(buf));
                if ((readLen == 1) && (buf[0] == 0))
                {
                    break; /* EOF */
                }

                if (readLen > 0)
                {
                    memcpy(&readBuf[readTotal], buf, readLen);
                    readTotal += readLen;

                    if (readTotal == size)
                    {
                        break;
                    }
                }
                else if (readLen < 0)
                {
                    break;
                }
            }
            else
                //(pollEvents.revents & POLLERR)   /* error */
            {
                qDebug() << Q_FUNC_INFO << ": serial connect error.";
                break;
            }
        }
        else
        {
            //qDebug() << "[SerPortRS485::readBytes]" << "no data to read." << pollState;
            break;
        }
    }
#endif

    //qDebug() << "[SerPortRS485::readBytes] read bytes:" << readTotal;
    return readTotal;
}

int SerPortRS485::writeBytes(char *dataBuf, int size)
{
    //qDebug() << "WRITE : " << dataBuf[0] << dataBuf[1] << dataBuf[2];
    return write(fd, dataBuf, size);
}

int SerPortRS485::config(long baudRate, int dataBits, int stopBits, int parity)
{
    struct termios newtio;
    int speed;

    if (!isPortOpen())
    {
        return (-1);
    }

    tcgetattr(fd, &oldtio);

    bzero(&newtio, sizeof(newtio));

    newtio.c_cflag = (CLOCAL | CREAD);
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;

    /* baudRate */
    switch (baudRate)
    {
        case 300:   speed = B300;
            break;
        case 600:   speed = B600;
            break;
        case 1200:  speed = B1200;
            break;
        case 1800:  speed = B1800;
            break;
        case 2400:  speed = B2400;
            break;
        case 4800:  speed = B4800;
            break;
        case 9600:  speed = B9600;
            break;
        case 19200: speed = B19200;
            break;
        case 38400: speed = B38400;
            break;
        case 57600: speed = B57600;
            break;
        case 115200:speed = B115200;
            break;
        case 230400:speed = B230400;
            break;
        case 460800:speed = B460800;
            break;
        default:
            return (-1);
    }

    if (cfsetispeed(&newtio, (speed_t) speed) < 0)
    {
        return (-1);
    }

    if (cfsetospeed(&newtio, (speed_t) speed) < 0)
    {
        return (-1);
    }

    /* dataBits */
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

    /* stopBits */
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

    /* parity */
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

    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        return (-1);
    }

    return 0;
}
