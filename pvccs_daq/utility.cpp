#include "buildopts.h"
#include "utility.h"

#include <time.h>
#include <sys/time.h>
#include <sys/statvfs.h>

#include <QList>
#include <QStringList>
#include <QDebug>
#include <QThread>

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
void Utility::msleep(int sleepMS)
{
    struct timespec ts;

    ts.tv_sec = sleepMS / 1000;
    ts.tv_nsec = (long)((sleepMS % 1000) * 1000 * 1000);
    nanosleep(&ts, NULL);
}

bool Utility::isCurTimeValid(QString timeStamp)
{
    bool ret = false;
    unsigned long long diff = 0;
    unsigned long long curTime   = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000;
    unsigned long long validTime = (time_t)timeStamp.toULongLong();

    diff = (curTime >= validTime) ? (curTime - validTime) : (validTime - curTime);

    if (diff <= 50)
        ret = true;

    //qDebug() << "[isCurTimeValid]" << ret << "curTime =" << curTime << "validTime:" << validTime;
    return ret;
}
#endif

#if 1
/* Table of CRC values for high-order byte */
static const unsigned char table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const unsigned char table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

ushort Utility::crc16(char *buf, int bufLen)
{
    char crcHi = 0xFF; /* high CRC byte initialized */
    char crcLo = 0xFF; /* low CRC byte initialized */
    uint i;              /* will index into CRC lookup */

    /* pass through message buffer */
    while (bufLen--)
    {
        i = crcHi ^ *buf++; /* calculate the CRC  */
        crcHi = crcLo ^ table_crc_hi[i];
        crcLo = table_crc_lo[i];
    }

    return (crcHi << 8 | crcLo);
}
#endif

QByteArray Utility::checkSum(QByteArray data)
{
    char check = 0;

	// qDebug() << "checkSum() ++";
	// qDebug() << "size = " << data.size();

    for (int i = 0; i < data.size(); i++) 
	{
        check = (char) (check ^ data.at(i));
    }

    QString checksum;
    checksum.sprintf("%02X", check);

	// qDebug() << "checkSum() --";

    return checksum.toLocal8Bit();
}

//static
QByteArray Utility::getType(QByteArray data)
{
    return data.split(',')[0];
}

//static
QByteArray Utility::getOwner(QByteArray data)
{
    return data.split(',')[1];
}

//static
QByteArray Utility::getCommand(QByteArray data)
{
    return data.split(',')[2];
}

//static
QByteArray Utility::getMessage(QByteArray data)
{
    return data.split(',')[3];
}

//static
QByteArray Utility::getParameter(QByteArray data)
{
    QList<QByteArray> parameters = data.split(',');
    parameters.removeAt(0);
    parameters.removeAt(0);
    parameters.removeAt(0);

    QStringList temp;
    foreach (QByteArray parameter, parameters) {
        temp.append(parameter);
    }

    return temp.join(",").toLocal8Bit();
}

//static
QMap<QTime,uint> Utility::convertStringToProfileItems(QString param)
{
    QMap<QTime,uint> map;
    QStringList items = param.split('|');

    if (items.size() == 1) 
	{
        if (items[0] == "") 
		{
            items.clear();
        }
    }

    //qDebug() << "[convertStringToProfileItems] " << items.size();

    if (items.size() > 0) 
	{
        foreach (QString item, items) 
        {
            //qDebug() << "[convertStringToProfileItems] " << item << items;
#if (BOPTS_FIX_QLIST_SIZE_ERROR == 1)
            if (item.isNull() == false)
            {
                QTime time = QTime::fromString(item.split('+')[0], "hh:mm");
                uint period = item.split('+')[1].toUInt();

                map.insert(time, period);
            }
#else
            QTime time = QTime::fromString(item.split('+')[0], "hh:mm");
            uint period = item.split('+')[1].toUInt();

            map.insert(time, period);
#endif
        }

        QList<QTime> keys = map.keys();
        qSort(keys.begin(), keys.end());
    }

    return map;
}

//static
QString Utility::convertProfileItemsToString(QMap<QTime,uint> param)
{
    QList<QTime> keys = param.keys();
    qSort(keys.begin(), keys.end());

    QStringList items;

    foreach (QTime key, keys) {
        QStringList item;
        item.append(key.toString("hh:mm"));
        item.append(QString::number(param.value(key)));

        items.append(item.join("+"));
    }

    return items.join("|");
}

//static
QByteArray Utility::makePacket(QString type, QString owner, QString command, QString param)
{
    QByteArray data;
    data.append(type);
    data.append(",");
    data.append(owner);
    data.append(",");
    data.append(command);
    data.append(",");
    data.append(param);

    QByteArray sum = checkSum(data);

    QByteArray packet;
    packet.append("^");
    packet.append(data);
    packet.append("*");
    packet.append(sum);
    packet.append("$");

    return packet;
}

//static
QByteArray Utility::makeReqPacket(QString owner, QString command, QString param)
{
    return makePacket("REQ", owner, command, param);
}

//static
QByteArray Utility::makeResPacket(QString owner, QString command, QString param)
{
    return makePacket("RES", owner, command, param);
}

//static
OsConfigInfo Utility::makeOsConfigInfoFromParameter(QByteArray param)
{
    QList<QByteArray> parameters = param.split(',');
    OsConfigInfo osConfigInfo;

    osConfigInfo.setPlantCode(parameters[0]);
    osConfigInfo.setFilterPressureHigh(parameters[1]);
    osConfigInfo.setPump1PressureLow(parameters[2]);
    osConfigInfo.setPump1PressureHigh(parameters[3]);
    osConfigInfo.setPump1CurrentShort(parameters[4]);
    osConfigInfo.setPump1CurrentOver(parameters[5]);
    osConfigInfo.setPump2PressureLow(parameters[6]);
    osConfigInfo.setPump2PressureHigh(parameters[7]);
    osConfigInfo.setPump2CurrentShort(parameters[8]);
    osConfigInfo.setPump2CurrentOver(parameters[9]);
    osConfigInfo.setSprayProfileName(parameters[10]);
    osConfigInfo.setSprayProfileItems(parameters[11]);
    osConfigInfo.setSprayManualTime(parameters[12]);
    osConfigInfo.setPumpNumber(parameters[13]);
    osConfigInfo.setSectorNumber(parameters[14]);
    osConfigInfo.setFreezeTemperature(parameters[15]);
    osConfigInfo.setCloudyTemperature(parameters[16]);
    osConfigInfo.setInverterMasterType(parameters[17]);
    osConfigInfo.setInverterProtocolType(parameters[18]);
    osConfigInfo.setInverterConnectionType(parameters[19]);
    osConfigInfo.setInverterDeviceName(parameters[20]);
    osConfigInfo.setInverterBaudrate(parameters[21]);
    osConfigInfo.setInverterIpAddress(parameters[22]);
    osConfigInfo.setInverterPortNumber(parameters[23]);
    osConfigInfo.setInverterSlaves(parameters[24]);
    osConfigInfo.setTemperatureCalibration(parameters[25]);
    osConfigInfo.setSolarPowerCalibration(parameters[26]);
    osConfigInfo.setVersion(parameters[27]);

    return osConfigInfo;
}

//static
DiskAmount Utility::getAmountInfo()
{
    DiskAmount amountInfo;
    struct statvfs stat;

    if (statvfs("/", &stat) == 0 ) {
        //long block_size = stat.f_bsize;
        amountInfo.total = stat.f_blocks * 4;
        amountInfo.used = (stat.f_blocks - stat.f_bfree) * 4;
        amountInfo.available =  stat.f_bavail * 4;
    }

    return amountInfo;
}
