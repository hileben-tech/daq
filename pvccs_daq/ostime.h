/******************************************************************************
 * Filename    : ostime.h
 * Description : Header of ostime.cpp
 * Create Date : 2016.03.31
 * Writter     : Moon, Sung Won
 *****************************************************************************/
#ifndef OSTIME_H
#define OSTIME_H

#include "buildopts.h"
#include <time.h>
#include <QObject>
#include <QThread>

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)

class COsTime : public QThread
{
    Q_OBJECT

public:
    explicit COsTime(QObject* parent = 0);

    bool isTimeUpdated();

signals:
    void logUpdateTimeResult(QString log);

public slots:
    void requestTimeUpdate();
    void updateOsTime(QString timeStamp);

private:
    time_t  m_tPrevTimeStamp;   /* Keep the previous timestamp to compare */
    bool    m_bTimeUpdated;
};

#endif
#endif /* OSTIME_H */
