/******************************************************************************
 * Filename    : ostime.cpp
 * Description : Class for BBB system time update
 * Create Date : 2016.03.31
 * Writter     : Moon, Sung Won
 *****************************************************************************/
#include "buildopts.h"
#include "ostime.h"
#include "context.h"
#include "utility.h"

#include <QObject>
#include <QDebug>
#include <time.h>
#include <sys/time.h>
#include <QDateTime>
#include <QSemaphore>
#include <QProcess>

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)

#define OSTIME_ENABLE_RECORD_LOG


COsTime::COsTime(QObject* parent) : QThread(parent)

{
    m_tPrevTimeStamp  = 0UL;
    m_bTimeUpdated    = false;
}

void COsTime::requestTimeUpdate()
{
    qDebug() << "[COsTime::requestTimeUpdate] ++ ";
    m_bTimeUpdated = false;
}

bool COsTime::isTimeUpdated()
{
    return m_bTimeUpdated;
}

void COsTime::updateOsTime(QString timeStamp)
{
    time_t setTime;
    struct tm *pTime;
    char buf[64];
    char logBuf[128] = {0};

    //qDebug() << "[COsTime::updateOSTime] ++ " << timeStamp << ", " << timeStamp.toULongLong();

    if (m_bTimeUpdated == false)
    {
        setTime  = (time_t)timeStamp.toULongLong();

        if (setTime > m_tPrevTimeStamp)
        {
            pTime = localtime(&setTime);
#if 0
            qDebug() << "YEAR  : " << (pTime->tm_year + 1900);
            qDebug() << "MONTH : " << (pTime->tm_mon + 1);
            qDebug() << "DAY   : " << (pTime->tm_mday);
            qDebug() << "HOUR  : " << (pTime->tm_hour);
            qDebug() << "MINUTE: " << (pTime->tm_min);
            qDebug() << "SECOND: " << (pTime->tm_sec);
#endif

            memset(buf, 0, sizeof(buf));
            //sprintf(buf, "date -s \"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\"", pTime->tm_year + 1900,
            sprintf(buf, "date -s \"%.4d%.2d%.2d %.2d:%.2d:%.2d\"", pTime->tm_year + 1900,
                                                                    pTime->tm_mon + 1,
                                                                    pTime->tm_mday,
                                                                    pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
            qDebug() << buf;

            system(buf);    /* Set system time */

            if (Utility::isCurTimeValid(timeStamp) ==  true)
            {
                m_tPrevTimeStamp = setTime;
                m_bTimeUpdated   = true;
                //qDebug() << "[COsTime::updateOSTime] OS system time updated. (" << buf << ")";

#if defined(OSTIME_ENABLE_RECORD_LOG)
                memset(logBuf, 0x00, sizeof(logBuf));
                sprintf(logBuf, "[O] UPDATED : %.4d%.2d%.2d %.2d:%.2d:%.2d", pTime->tm_year + 1900,
                                                                        pTime->tm_mon + 1,
                                                                        pTime->tm_mday,
                                                                        pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
#endif
            }
            else
            {
                time_t curTime = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000;
                qDebug() << "[COsTime::updateOSTime] ERROR : Time update failed.(req:" << setTime << ", cur:" << curTime << ")";

#if defined(OSTIME_ENABLE_RECORD_LOG)
                memset(logBuf, 0x00, sizeof(logBuf));
                sprintf(logBuf, "[X] FAILED  : new(%lu) cur(%lu)", setTime, curTime);
#endif
            }
        }
        else
        {
            m_bTimeUpdated = false;
            qDebug() << "[COsTime::updateOSTime] Err : Bad timestamp. (prev:" << m_tPrevTimeStamp << ", now:" << setTime << ")";

#if defined(OSTIME_ENABLE_RECORD_LOG)
            memset(logBuf, 0x00, sizeof(logBuf));
            sprintf(logBuf, "[X] FAILED  : Bad timestamp.(prev:%lu, req:%lu)", m_tPrevTimeStamp, setTime);
#endif
        }

#if defined(OSTIME_ENABLE_RECORD_LOG)
        emit logUpdateTimeResult(logBuf);
#endif
    }
    else
    {
#if defined(OSTIME_ENABLE_RECORD_LOG)
        memset(logBuf, 0x00, sizeof(logBuf));
        sprintf(logBuf, "[#] NOAPPLY : %d",(m_bTimeUpdated == true) ? 1 : 0);
        emit logUpdateTimeResult(logBuf);
#endif
    }

    //qDebug() << "[COsTime::updateOSTime] -- " << timeStamp;
}

#endif
