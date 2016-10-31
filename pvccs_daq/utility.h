#ifndef UTILITY_H
#define UTILITY_H

#include "osconfiginfo.h"

#include <QString>
#include "buildopts.h"
#include <QByteArray>
#include <QMap>
#include <QTime>
#include <QList>
#include <QMutex>

struct DiskAmount {
    long total;
    long used;
    long available;

    inline QString toString()
    {
        return QString("total: %1, used: %2, available: %3")
                .arg(QString::number(total))
                .arg(QString::number(used))
                .arg(QString::number(available));
    }
};

class Utility
{
    Utility();

public:
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    static void msleep(int sleepMS);
    static bool isCurTimeValid(QString timeStamp);
#endif
    static ushort crc16(char *buf, int bufLength);

    static QByteArray checkSum(QByteArray data);

    static QByteArray getType(QByteArray data);
    static QByteArray getOwner(QByteArray data);
    static QByteArray getCommand(QByteArray data);
	static QByteArray getMessage(QByteArray data);
    static QByteArray getParameter(QByteArray data);

    static QMap<QTime,uint> convertStringToProfileItems(QString param);
    static QString convertProfileItemsToString(QMap<QTime,uint> param);

    static QByteArray makePacket(QString type, QString own, QString cmd, QString param);
    static QByteArray makeReqPacket(QString own, QString cmd, QString param);
    static QByteArray makeResPacket(QString own, QString cmd, QString param);

    static OsConfigInfo makeOsConfigInfoFromParameter(QByteArray param);

    static DiskAmount getAmountInfo();

    //static void msleep(long sleepMS);
};

#endif // UTILITY_H
