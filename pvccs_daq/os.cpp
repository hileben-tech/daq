#include "buildopts.h"
#include "os.h"
#include "ostime.h"
#include "context.h"
#include "statusinfo.h"
#include "ledinfo.h"
#include "utility.h"
#include "version.h"
#include "sckcmodbus.h"
#include <QProcess>
#include <QList>
#include <QDebug>
#include <modbus.h>

OS::OS(QObject *parent) :
    QThread(parent)
{
    updateProfileItems();

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    m_sendStatusToWS = false;
#endif
#if (BOPTS_FIX_AUTO_SPRAY_NOT_WORK_AFTER_RESETTING_SPRAYTIMETABLE_ERROR == 1)
    profileItemsUpdated = false;
#endif

    logger = new Logger(this);

    connect(this, SIGNAL(changedStatusInfo(StatusInfo,QDateTime)), logger, SLOT(logMessage(StatusInfo,QDateTime)));
    connect(this, SIGNAL(ignoredCommand(QByteArray,QDateTime,QString)), logger, SLOT(logMessage(QByteArray,QDateTime,QString)));

    Context* ctx = Context::getInstance();

    if (!ctx->getUseInverterOnly()) {
        //qDebug() << "\n\tinitialize i/o controller";

        ioController = new IOController(this);
    }
    else {
        ioController = 0;
    }

    if (ioController) {
        connect(ioController, SIGNAL(sentPacket(QByteArray,QDateTime,QString)), logger, SLOT(logMessage(QByteArray,QDateTime,QString)));
        connect(ioController, SIGNAL(receivedPacket(QByteArray,QDateTime,QString)), logger, SLOT(logMessage(QByteArray,QDateTime,QString)));
        connect(ioController, SIGNAL(ignoredPacket(QByteArray,QDateTime,QString)), logger, SLOT(logMessage(QByteArray,QDateTime,QString)));
    }

    if (!ctx->getUseInverterOnly()) {
        //qDebug() << "\n\tinitialize i/o database";

        ioDatabase = new IODatabase(this);
    }
    else {
        ioDatabase = 0;
    }

    if (ioDatabase) {
        connect(ioDatabase, SIGNAL(insertedStatusInfo(QString,QDateTime,QString)), logger, SLOT(logMessage(QString,QDateTime,QString)));
        connect(ioDatabase, SIGNAL(deletedStatusInfo(QString,QDateTime,QString)), logger, SLOT(logMessage(QString,QDateTime,QString)));
        connect(ioDatabase, SIGNAL(selectedStatusInfo(QString,QDateTime,QString)), logger, SLOT(logMessage(QString,QDateTime,QString)));
    }

    invMediator = new InvMediator(this);

    connect(invMediator, SIGNAL(readyInvInfo(InvInfo,QDateTime)), logger, SLOT(logMessage(InvInfo,QDateTime)));

    invDatabase = new InvDatabase(this);

    connect(invDatabase, SIGNAL(insertedInvInfo(QString,QDateTime,QString)), logger, SLOT(logMessage(QString,QDateTime,QString)));
    connect(invDatabase, SIGNAL(deletedInvInfo(QString,QDateTime,QString)), logger, SLOT(logMessage(QString,QDateTime,QString)));
    connect(invDatabase, SIGNAL(selectedInvInfo(QString,QDateTime,QString)), logger, SLOT(logMessage(QString,QDateTime,QString)));

    webClient = new WebClient(this);

    connect(webClient, SIGNAL(sentPacket(QByteArray,QDateTime,QString)), logger, SLOT(logMessage(QByteArray,QDateTime,QString)));
    connect(webClient, SIGNAL(receivedPacket(QByteArray,QDateTime,QString)), logger, SLOT(logMessage(QByteArray,QDateTime,QString)));
    connect(webClient, SIGNAL(ignoredPacket(QByteArray,QDateTime,QString)), logger, SLOT(logMessage(QByteArray,QDateTime,QString)));

    setterServer = new SetterServer(this);

    connect(setterServer, SIGNAL(connectedSetter(QByteArray,QDateTime,QString)), logger, SLOT(logMessage(QByteArray,QDateTime,QString)));
    connect(setterServer, SIGNAL(disconnectedSetter(QByteArray,QDateTime,QString)), logger, SLOT(logMessage(QByteArray,QDateTime,QString)));
    connect(setterServer, SIGNAL(receivedPacket(QByteArray,QDateTime,QString)), logger, SLOT(logMessage(QByteArray,QDateTime,QString)));
    connect(setterServer, SIGNAL(sentPacket(QByteArray,QDateTime,QString)), logger, SLOT(logMessage(QByteArray,QDateTime,QString)));

    /*
     * flow
     */
    if (ioController) {
        connect(ioController, SIGNAL(readyStatusInfo(StatusInfo)), this, SLOT(onReadyStatusInfo(StatusInfo)));
        connect(ioController, SIGNAL(timeout()), this, SLOT(onIoTimeout()));
    }

    if (ioDatabase) {
        connect(this, SIGNAL(changedStatusInfo(StatusInfo,QDateTime)), ioDatabase, SLOT(insertStatusInfo(StatusInfo,QDateTime)));
        connect(ioDatabase, SIGNAL(readyParameter(QByteArray)), webClient, SLOT(onReadyIoParameter(QByteArray)));
    }

    connect(invMediator, SIGNAL(readyInvInfo(InvInfo,QDateTime)), this, SLOT(onReadyInvInfo(InvInfo)));
    connect(invMediator, SIGNAL(readyInvInfo(InvInfo,QDateTime)), invDatabase, SLOT(insertInvInfo(InvInfo,QDateTime)));
    connect(invDatabase, SIGNAL(readyParameter(QByteArray)), webClient, SLOT(onReadyInvParameter(QByteArray)));

    if (ioDatabase) {
        connect(webClient, SIGNAL(insertedStatusInfo(QString)), ioDatabase, SLOT(deleteStatusInfo(QString)));
    }

    connect(webClient, SIGNAL(insertedInvInfo(QString)), invDatabase, SLOT(deleteInvInfo(QString)));
    connect(webClient, SIGNAL(readyCommand(QByteArray,QByteArray)), this, SLOT(processCommand(QByteArray,QByteArray)));
    connect(webClient, SIGNAL(receivedPacket(QByteArray,QDateTime,QString)), this, SLOT(onWsTimein()));
    connect(webClient, SIGNAL(timeout()), this, SLOT(onWsTimeout()));
    connect(setterServer, SIGNAL(readyCommand(QByteArray,QByteArray)), this, SLOT(processCommand(QByteArray,QByteArray)));

    connect(invMediator, SIGNAL(changedInvConfigInfo(InvConfigInfo)), this, SLOT(resetInverterStatus(InvConfigInfo)));


#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    m_OsTime = new COsTime(this);
    if (m_OsTime)
    {
        connect(webClient, SIGNAL(requestTimeUpdate()), m_OsTime, SLOT(requestTimeUpdate()));
        connect(webClient, SIGNAL(receivedWebTime(QString)), m_OsTime, SLOT(updateOsTime(QString)));
        connect(m_OsTime, SIGNAL(logUpdateTimeResult(QString)), logger, SLOT(logTimeMessage(QString)));
        connect(this, SIGNAL(logUpdateTimeResult(QString)), logger, SLOT(logTimeMessage(QString)));
    }
#else
	/* ===================================== */
	/* ===== hmbae ========================= */
	/* ===================================== */
	m_UdpClient = new CUdpClient(this);
	m_UdpClient->Init();
	/* ===================================== */
	/* ===================================== */
#endif

#if (BOPTS_ADD_FUNCTIONS_FOR_DAQ == 1)

    sckcModbus = new SckCModbus;

    int valid = 0;
    int retry = 5;

    retry = 0x0FFFFFFF;
    while (retry--)
    {
        if (sckcModbus->connect(SCKCMODBUS_PORT_NAME, SCKCMODBUS_BAUDRATE) == 0)
        {
            Utility::msleep(500);
            if (sckcModbus->setSlave(SCKCMODBUS_SLAVE_ADDRESS) == 0)
            {
                Utility::msleep(500);

                if (sckcModbus->checkChConnection() > 0)
                {
                    retry = 0;
                    valid = 1;
                }
                else
                {
                    sckcModbus->disconnect();
                    Utility::msleep(500);
                }
            }
        }
    }

    if (valid == 0)
    {
        delete sckcModbus;
        sckcModbus = NULL;
        qDebug() << "[OS::OS] SCK-C-MODBUS connect failed.(" << SCKCMODBUS_PORT_NAME << SCKCMODBUS_BAUDRATE << ")";
    }
#endif


    waitPumpRunning = NULL;
    pumpRunning = false;

    ioBoardTimeoutCount = 0;

    before_dateTime = QDateTime::currentDateTime();

    initializeIoBoardReset();
}

bool OS::isEmergency()
{
    return (statusInfo.getOsShutdown().toInt() == LedInfo::LED_RED);
}

bool OS::isSpraying()
{
    return (statusInfo.getWaterSpray().toInt() == LedInfo::LED_GREEN);
}

bool OS::isDraining()
{
    return (statusInfo.getWaterDrain().toInt() == LedInfo::LED_GREEN);
}

bool OS::isFreezing()
{
    return (statusInfo.getWarnFrozen().toInt() == LedInfo::LED_GREEN);
}

bool OS::isCloudy()
{
	return (statusInfo.getCloudy().toInt() == LedInfo::LED_RED);
}

bool OS::isPumpRunning()
{
    return ((statusInfo.getPump1().toInt() == LedInfo::LED_GREEN) || (statusInfo.getPump2().toInt() == LedInfo::LED_GREEN));
}

void OS::updateProfileItems()
{
    OsConfigInfo osConfigInfo = Context::getInstance()->getOsConfigInfo();

#if (BOPTS_FIX_AUTO_SPRAY_NOT_WORK_AFTER_RESETTING_SPRAYTIMETABLE_ERROR == 1)
    mutexProfileItems.lock();
#endif
    //qDebug() << "############################################ 0";

    profileItems.clear();
    profileItems = Utility::convertStringToProfileItems(osConfigInfo.getSprayProfileItems());

    //qDebug() << "############################################ 1";
#if (BOPTS_FIX_AUTO_SPRAY_NOT_WORK_AFTER_RESETTING_SPRAYTIMETABLE_ERROR == 1)
    profileItemsUpdated = true;
    mutexProfileItems.unlock();
#endif

    QTime now = QTime::currentTime();
    QList<QTime> keys = profileItems.keys();

    itemIndex = 0;

    foreach (QTime time, keys) 
	{
        if (now.msecsTo(time) > 0) 
		{
            break;
        }

        itemIndex++;
    }

    //qDebug() << "\n\titem index: " << itemIndex;
}

void OS::initializeIoBoardReset()
{
    ioBoardReset = new SysfsGpio(67);
    ioBoardReset->setDirection("out");
    ioBoardReset->setValue("1");

    pendCheckingFrozen = new QTimer(this);
    connect(pendCheckingFrozen, SIGNAL(timeout()), this, SLOT(starCheckFrozen()));
    pendCheckingFrozen->start(10 * 1000);
}

void OS::resetIoBoard()
{
    qDebug() << "\n\nresetIoBoard()++"; 
		
    ioBoardReset->setValue("0");

    QThread::msleep(1);

    ioBoardReset->setValue("1");

    qDebug() << "resetIoBoard()--\n\n";
}

void OS::resetInverterStatus(InvConfigInfo invConfigInfo)
{
    LedInfo temp = extract(statusInfo);

    temp.setInverter1("0");
    temp.setInverter2("0");
    temp.setInverter3("0");
    temp.setInverter4("0");

    if (temp.toString() != extract(statusInfo).toString()) {
        processCommand("BBB", "LED", temp.toString().toLocal8Bit());

        statusInfo = reflect(temp, statusInfo);

        emit changedStatusInfo(statusInfo, QDateTime::currentDateTime());
    }
}

LedInfo OS::extract(StatusInfo param)
{
    LedInfo ledInfo;

    ledInfo.setWarnFrozen(param.getWarnFrozen());
    ledInfo.setCloudy(param.getCloudy());
    ledInfo.setIoBoard(param.getIoBoard());
    ledInfo.setInverter1(param.getInverter1());
    ledInfo.setInverter2(param.getInverter2());
    ledInfo.setInverter3(param.getInverter3());
    ledInfo.setInverter4(param.getInverter4());
    ledInfo.setWebServer(param.getWebServer());
    ledInfo.setFilterPressureSensor(param.getFilterPressureSensor());
    ledInfo.setPump1PressureSensor(param.getPump1PressureSensor());
    ledInfo.setPump2PressureSensor(param.getPump2PressureSensor());
    ledInfo.setPump1CtModule(param.getPump1CtModule());
    ledInfo.setPump2CtModule(param.getPump2CtModule());

    return ledInfo;
}

StatusInfo OS::reflect(LedInfo src, StatusInfo dest)
{
    dest.setWarnFrozen(src.getWarnFrozen());
    dest.setCloudy(src.getCloudy());
    dest.setIoBoard(src.getIoBoard());
    dest.setInverter1(src.getInverter1());
    dest.setInverter2(src.getInverter2());
    dest.setInverter3(src.getInverter3());
    dest.setInverter4(src.getInverter4());
    dest.setWebServer(src.getWebServer());
    dest.setFilterPressureSensor(src.getFilterPressureSensor());
    dest.setPump1PressureSensor(src.getPump1PressureSensor());
    dest.setPump2PressureSensor(src.getPump2PressureSensor());
    dest.setPump1CtModule(src.getPump1CtModule());
    dest.setPump2CtModule(src.getPump2CtModule());

    return dest;
}

/* ================================================ */
/* ======= hmbae ================================== */
/* ================================================ */
#define OS_RUN_500MS
#define CONNECT_RETRY_TIME        (10 * 4)  /* 10 Seconds */

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
#if defined(OS_RUN_500MS)
    #define OS_WEBTIME_REQ_RETRY_PERIOD     (2 * 5)         /* 5 sec */
#if (BOPTS_SEND_STATUS_FASTER == 1)
    #define OS_STATUS_SEND_PERIOD           (2 * 10)        /* 10 secs : send Status from OS->WS */
#else
    #define OS_STATUS_SEND_PERIOD           (2 * 60)        /* 1 min : send Status from OS->WS */
#endif // #if (BOPTS_SEND_STATUS_FASTER == 1)

#else  // #if defined(OS_RUN_500MS)
    #define OS_WEBTIME_REQ_RETRY_PERIOD     (4 * 5)         /* 5 sec */
    #define OS_STATUS_SEND_PERIOD           (4 * 60)        /* 1 min */
#endif // #if defined(OS_RUN_500MS)
#endif // #if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)

#if (BOPTS_ADD_FUNCTIONS_FOR_DAQ == 1)
    #define OS_SCKCMODBUS_READ_DATA_PERIOD  (2 * 15)
    #define OS_SCKCMODBUS_SEND_DATA_PERIOD  (2 * 30)
#endif

int  OS::GetUpdateTimeInterval()
{
    int Interval = 0;

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 0)
#if defined(OS_RUN_500MS)
	if( m_UdpClient->IsTimeUpdated() == TRUE )
	{
		Interval = (4 * 60 * 60 * 2);   /* 4 Hours */
	}
	else
	{
        Interval = (60 * 2);  /* 60 Seconds */
	}
#else
	if( m_UdpClient->IsTimeUpdated() == TRUE )
	{
		Interval = (4 * 60 * 60 * 4);   /* 4 Hours */
	}
	else
	{
		Interval = (60 * 4);  /* 60 Seconds */
	}
#endif //
#endif

	return Interval;
}
/* ================================================ */


//virtual
void OS::run()
{
    pendedCheckFrozen = false;

    Context* ctx = Context::getInstance();

	QTime thisTime;
    QTime sprayTime;
    int   prevIndex = -1;
	int   i;
#if (BOPTS_FIX_AUTO_SPRAY_NOT_WORK_AFTER_RESETTING_SPRAYTIMETABLE_ERROR == 1)
    QTime prevSprayTime(0, 0, 0);
    QDate prevSprayDay(1, 1, 1);
    QDate thisDay;
#endif
#if (BOPTS_ADD_FUNCTIONS_FOR_DAQ == 1)
    uint readSckCModbusCnt = 0, sendSckCModbusCnt = 0;
    bool readSckCModbusData = false, sendSckCModbusData = false;
#endif

	/* ================================================ */
	/* ======= hmbae ================================== */
	/* ================================================ */
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    unsigned int nCount = 0;
    unsigned int nSendStatusCount = 0;
    unsigned int profileItemsSize = 0;
#else
    int  nUpdateTimeInterval = GetUpdateTimeInterval();
    int  nCount = nUpdateTimeInterval;
	int  nConnectRetry = CONNECT_RETRY_TIME;
	bool bConnectRetry = FALSE;
#endif
    /* ================================================ */

#if (BOPTS_ADD_FUNCTIONS_FOR_DAQ == 1)
    /**************************************************************************
     * SCK-C-MODBUS
     *************************************************************************/
    if (sckcModbus)
    {
        qDebug() << "====================================================";

        /* Read Communication module name & serial No. */
        int loopCnt = 30;
        while (loopCnt > 0)
        {
           if ((sckcModbus->readModuleName(0).isEmpty() == false) && (sckcModbus->readSerialNumber(0).isEmpty() == false))
            {
                qDebug() << "= DAQ MODULE INFOS";
                qDebug() << "= Comm Module:" << sckcModbus->getModuleName(0) << sckcModbus->getSerialNum(0);
                break;
            }
            loopCnt--;
            Utility::msleep(100);
            qDebug() << "Trying Module..." << loopCnt;
        }

        if (loopCnt > 0)
        {
            /* Read channel information. Valid info only if channel is connected. */
            loopCnt = 30;
            while (loopCnt > 0)
            {
                int readChCnt = 0;
                for (int ch = 1; ch <= 8; ch++)
                {
                    if (sckcModbus->readChInfo(ch) == 0)
                    {
                        qDebug() << "= [ CH" << ch << "]" << sckcModbus->getModuleName(ch) << sckcModbus->getSerialNum(ch);
                        readChCnt++;
                    }
                }

                if (readChCnt > 0)
                {
                    break;
                }

                loopCnt--;
                Utility::msleep(100);
                qDebug() << "Trying Channel..." << loopCnt;
            }
        }
        qDebug() << "====================================================";
    }
#endif


    for (;;) 
	{
        {
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)

            if (m_OsTime->isTimeUpdated() == false)
            {
                if ((nCount == 0) || (nCount >= OS_WEBTIME_REQ_RETRY_PERIOD))
                {
                    webClient->sendTimeReqPacket();
                    nCount = 0;
                    qDebug() << "[OS::run] Try Time Update : sendTimeReqPacket...";
                }
                nCount++;
            }
            else
            {
                nCount = 0;
            }

#else
			/* ================================================ */
			/* ======= hmbae ================================== */
			/* ================================================ */
			nUpdateTimeInterval = GetUpdateTimeInterval();

            //qDebug() << "===============  " << bConnectRetry << m_UdpClient->IsConnected();
            /* ====== hmbae code =============== */
			if( bConnectRetry == TRUE && m_UdpClient->IsConnected() == FALSE )
			{
				if( nConnectRetry >= CONNECT_RETRY_TIME )
				{
					m_UdpClient->ConnectToTimeServer();
					nConnectRetry = 0;
				}
				else
				{
					nConnectRetry++;
				}
			}

			
			nCount++;
			if( nCount >= nUpdateTimeInterval )
			{
				// printf("============= UpdateTime() ============= \n");
				if( m_UdpClient->IsConnected() == TRUE )
				{
					bConnectRetry = FALSE;
					m_UdpClient->UpdateTime();
					nCount = 0;
					nConnectRetry = CONNECT_RETRY_TIME;
				}
				else
				{
					bConnectRetry = TRUE;
					nCount = nUpdateTimeInterval;
				}
			}
#endif


			/* ================================================ */
			/* ======= hmbae END=============================== */
			/* ================================================ */
            //qDebug() << "OS::Run ++";
            processCommand("BBB", "STATUS");   /* Get status from IO */


#if (BOPTS_ADD_FUNCTIONS_FOR_DAQ == 1)
            /**************************************************************************
             *
             *  SCK-C-MODBUS work for DAQ. No Timetable work.
             *
             *************************************************************************/
            if (1)//(m_OsTime->isTimeUpdated() == true)
            {
                if (readSckCModbusData == true)
                {
                    if (sckcModbus)
                    {
                        for (int ch = 1; ch <= 8; ch++)
                        {
                            if (sckcModbus->isChConnected(ch) == true)
                            {
                                sckcModbus->readCurrent(ch);
                                sckcModbus->readVoltage(ch);
                            }
                        }
                        readSckCModbusData = false;
                    }
                }

                if (sendSckCModbusData == true)
                {
                    if (sckcModbus)
                    {
                        webClient->sendStatusDAQPacket(sckcModbus);
                        sendSckCModbusData = false;
                    }
                }

                if (readSckCModbusData == false)
                {
                    readSckCModbusData = (++readSckCModbusCnt >= OS_SCKCMODBUS_READ_DATA_PERIOD) ? true : false;
                    if (readSckCModbusData == true)
                    {
                        readSckCModbusCnt = 0;
                    }
                }

                if (sendSckCModbusData == false)
                {
                    sendSckCModbusData = (++sendSckCModbusCnt >= OS_SCKCMODBUS_SEND_DATA_PERIOD) ? true : false;
                    if (sendSckCModbusData == true)
                    {
                        sendSckCModbusCnt = 0;
                    }
                }
            }

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
            if (m_sendStatusToWS == false)
            {
                m_sendStatusToWS = (++nSendStatusCount >= OS_STATUS_SEND_PERIOD) ? true : false;
                if (m_sendStatusToWS == true)
                {
                    //qDebug() << "[OS::run] Send STATUS to WS..." << nSendStatusCount;
                    nSendStatusCount = 0;
                }
            }
#endif

            Utility::msleep(500);

#else
            /**************************************************************************
             *
             *  TimeTable work.
             *
             *************************************************************************/

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    #if (BOPTS_FIX_AUTO_SPRAY_NOT_WORK_AFTER_RESETTING_SPRAYTIMETABLE_ERROR == 1)
            mutexProfileItems.lock();
    #endif
            profileItemsSize = profileItems.size();

    #if (BOPTS_FIX_AUTO_SPRAY_NOT_WORK_AFTER_RESETTING_SPRAYTIMETABLE_ERROR == 1)
            if (profileItemsUpdated == true)
            {
                profileItemsUpdated = false;
            }
            mutexProfileItems.unlock();
    #endif

            if( (m_OsTime->isTimeUpdated() == true) && (profileItemsSize > 0) )
#else
			if( m_UdpClient->IsTimeUpdated() == TRUE && profileItems.size() > 0)
#endif
            {
                // QThread::msleep(250);
				// QThread::msleep(1000);

#if (BOPTS_FIX_AUTO_SPRAY_NOT_WORK_AFTER_RESETTING_SPRAYTIMETABLE_ERROR == 1)
                thisDay  = QDate::currentDate();
#endif
                thisTime = QTime::currentTime();

#if (BOPTS_FIX_AUTO_SPRAY_NOT_WORK_AFTER_RESETTING_SPRAYTIMETABLE_ERROR == 1)

                i = 0;
                while (1)
                {
                    mutexProfileItems.lock();
                    //qDebug() << "========================================= 0";

                    if ((i >= profileItems.size())          // No more items.
                        || (profileItemsUpdated == true))   // ProfileItems updated! Need to restart.
                    {
                        mutexProfileItems.unlock();
                        //qDebug() << "========================================= 1111";
                        break;
                    }

                    sprayTime = profileItems.keys()[i];

                    //qDebug() << i << "SparyTime =" << sprayTime.hour() << ":" << sprayTime.minute() << ", CurTime =" << thisTime.hour() << ":" << thisTime.minute();
                    //qDebug() << i << "SparyDay =" << thisDay.month() << "/" << thisDay.day() << "CurDay =" << thisDay.month() << "/" << thisDay.day();

                    if( (sprayTime.hour() == thisTime.hour()) && (sprayTime.minute() == thisTime.minute()) )
                    {
                        if( (prevSprayDay.month() != thisDay.month())
                            || (prevSprayDay.day() != thisDay.day())
                            || (prevSprayTime.hour() != sprayTime.hour())
                            || (prevSprayTime.minute() != sprayTime.minute()) )
                        {
                            qDebug() << "prevSprayTime = " << prevSprayTime.hour() << ":" << prevSprayTime.minute();

                            processCommand("BBB", "A-SPRAY", QString::number(profileItems.value(sprayTime)).toLocal8Bit());

                            prevSprayTime = sprayTime;
                            prevSprayDay  = thisDay;
                        }
                    }

                    i++;
                    //qDebug() << "========================================= 1";
                    mutexProfileItems.unlock();
                }

#else
                for( i = 0 ; i < profileItems.size() ; i ++ )
                {
                    sprayTime = profileItems.keys()[i];

                    //qDebug() << i << "SparyTime =" << sprayTime.hour() << ":" << sprayTime.minute() << ", CurTime =" << thisTime.hour() << ":" << thisTime.minute();
                    //qDebug() << i << "SparyDay =" << thisDay.month() << "/" << thisDay.day() << "CurDay =" << thisDay.month() << "/" << thisDay.day();

                    if( (sprayTime.hour() == thisTime.hour()) && (sprayTime.minute() == thisTime.minute()) )
                    {
                        if( prevIndex != i )
                        {
                            // qDebug() << "prevIndex = " << prevIndex;
                            // qDebug() << "i = " << i;

                            processCommand("BBB", "A-SPRAY", QString::number(profileItems.value(sprayTime)).toLocal8Bit());

                            prevIndex = i;
                        }
                    }
                }
#endif

				
#if defined(OS_RUN_500MS)
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
                Utility::msleep(500);
#else
                QThread::msleep(500);
#endif

#else
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
                Utility::msleep(250);
 #else
                QThread::msleep(250);
#endif
#endif //
			}
			else
			{
#if 0//(BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
                if (m_OsTime->IsTimeUpdated() == FALSE)
                {
                    char logBuf[64] = {0};
                    sprintf(logBuf, "Waiting update...(%d, %d)", nCount, (m_OsTime->IsTimeUpdated() == TRUE) ? 1 : 0);
                    emit logUpdateTimeResult(logBuf);
                    qDebug() << "[OS::run] Waiting os time update..." << nCount;
                }

#endif
                //qDebug() << "[OS::run] Waiting os time update..." << nCount;

#if defined(OS_RUN_500MS)
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
                Utility::msleep(500);
#else
                QThread::msleep(500);
#endif
#else
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
                Utility::msleep(250);
#else
                QThread::msleep(250);
#endif
#endif //
            }

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
            if (m_sendStatusToWS == false)
            {
                m_sendStatusToWS = (++nSendStatusCount >= OS_STATUS_SEND_PERIOD) ? true : false;
                if (m_sendStatusToWS == true)
                {
                    //qDebug() << "[OS::run] Send STATUS to WS..." << nSendStatusCount;
                    nSendStatusCount = 0;
                }
            }
            else
            {
                nSendStatusCount = 0;
            }
#endif

#endif  /*********************************************************************/
        }
    }
    qDebug() << "[OS::run] --";
}


void OS::DumpStatusInfo(StatusInfo Info)
{
	qDebug() << "==== Dump StatusInfo ======";
    qDebug() << "filterPressure ="  << Info.filterPressure;
    qDebug() << "pump1Pressure ="  << Info.pump1Pressure;
    qDebug() << "pump2Pressure ="  << Info.pump2Pressure;
    qDebug() << "pump1Ct ="  << Info.pump1Ct;
    qDebug() << "pump2Ct ="  << Info.pump2Ct;
    qDebug() << "radiation ="  << Info.radiation;
    qDebug() << "installedTemperature ="  << Info.installedTemperature;
    qDebug() << "uninstalledTemperature ="  << Info.uninstalledTemperature;
    qDebug() << "airTemperature ="  << Info.airTemperature;

    /*
     * status
     */
    qDebug() << "supplyValve ="  << Info.supplyValve;
    qDebug() << "waterLevelH ="  << Info.waterLevelH;
    qDebug() << "waterLevelM ="  << Info.waterLevelM;
    qDebug() << "waterLevelL ="  << Info.waterLevelL;
    qDebug() << "saltLevelL ="  << Info.saltLevelL;
    qDebug() << "indraftValve ="  << Info.indraftValve;
    qDebug() << "pump1 ="  << Info.pump1;
    qDebug() << "pump2 ="  << Info.pump2;
    qDebug() << "drainValve ="  << Info.drainValve;
    qDebug() << "sector1Valve ="  << Info.sector1Valve;
    qDebug() << "sector2Valve ="  << Info.sector2Valve;
    qDebug() << "sector3Valve ="  << Info.sector3Valve;
    qDebug() << "sector4Valve ="  << Info.sector4Valve;
    qDebug() << "sector5Valve ="  << Info.sector5Valve;
    qDebug() << "doorOpen ="  << Info.doorOpen;
    qDebug() << "osShutdown ="  << Info.osShutdown;
    qDebug() << "waterSpray ="  << Info.waterSpray;
    qDebug() << "waterDrain ="  << Info.waterDrain;
    qDebug() << "warnWaterLevelL ="  << Info.warnWaterLevelL;
    qDebug() << "warnSaltLevelL ="  << Info.warnSaltLevelL;

    qDebug() << "warnFrozen ="  << Info.warnFrozen;
    qDebug() << "cloudy ="  << Info.cloudy;
    qDebug() << "ioBoard ="  << Info.ioBoard;
    qDebug() << "inverter1 ="  << Info.inverter1;
    qDebug() << "inverter2 ="  << Info.inverter2;
    qDebug() << "inverter3 ="  << Info.inverter3;
    qDebug() << "inverter4 ="  << Info.inverter4;
    qDebug() << "webServer ="  << Info.webServer;
    qDebug() << "filterPressureSensor ="  << Info.filterPressureSensor;
    qDebug() << "pump1PressureSensor ="  << Info.pump1PressureSensor;
    qDebug() << "pump2PressureSensor ="  << Info.pump2PressureSensor;
    qDebug() << "pump1CtModule ="  << Info.pump1CtModule;
    qDebug() << "pump2CtModule ="  << Info.pump2CtModule;
	qDebug() << "====================";
}

#if 1
void OS::onReadyStatusInfo(StatusInfo param)
{
	bool   bUpdated = false;

	// qDebug() << "onReadyStatusInfo()++\n"; 

    ioBoardTimeoutCount = 0;

    OsConfigInfo osConfigInfo = Context::getInstance()->getOsConfigInfo();
    LedInfo temp = extract(param);

	// DumpStatusInfo(param);

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    /*
     */
    if (m_sendStatusToWS == true)
    {
        bUpdated = true;
        m_sendStatusToWS = false;
    }

#else
	// qDebug() << "onReadyStatusInfo() 8\n"; 
	QString tempTime = QDateTime::currentDateTime().toString("hh");
    if( tempTime.toInt() > 22 || tempTime.toInt() < 5 )  //   if 22 hour ~ 4 hour, then save status info per ten minute 
	{
	#if 0
		if( compareStatusInfo(param) ) /* Status is changed */
		{
			qDebug() << "Web Update : Status Changed !!!";
			bUpdated = false;
		}
		else
	#endif // 		
		if( before_dateTime.secsTo(QDateTime::currentDateTime()) > 60*STATUS_UPDATE_NIGHT_PERIOD )
		{
			before_dateTime = QDateTime::currentDateTime();
			// qDebug() << "Web Update : Timeout !!!";
			bUpdated = true;
		}
    } 
	else // if( compareStatusInfo(param) || ( before_dateTime.secsTo(QDateTime::currentDateTime()) > 60 * 5 ) ) 
	{
	#if 0
		if( compareStatusInfo(param) ) /* Status is changed */
		{
			qDebug() << "Web Update : Status Changed !!!";
			bUpdated = false;
		}
		else
	#endif // 		
		if( before_dateTime.secsTo(QDateTime::currentDateTime()) > 60 * STATUS_UPDATE_DAY_PERIOD )
		{
			before_dateTime = QDateTime::currentDateTime();
			// qDebug() << "Web Update : Timeout !!!";
			bUpdated = true;
		}
	}
#endif

	statusInfo = param;
	    
    //qDebug() << "Air Temp=" << param.getAirTemperature() << param.getAirTemperature().toInt();
    //qDebug() << "Freeze Temp=" << osConfigInfo.getFreezeTemperature().toInt();
    //qDebug() << "Installed Temp=" << param.getInstalledTemperature();
	//qDebug() << "Uninstalled Temp=" << param.getUninstalledTemperature();
	//qDebug() << "onReadyStatusInfo() 1\n"; 

#if (BOPTS_FIX_VERSION_INFO == 1)
    if (Version::getIOVersion() != statusInfo.getIOVersion())
    {
        qDebug() << "==================================";
        qDebug() << " IO Version : " << statusInfo.getIOVersion();
        qDebug() << "==================================";

        Version::setIOVersion(statusInfo.getIOVersion());
    }
#endif

    if (!isEmergency()) 
	{
        if (waitPumpRunning == NULL) 
		{
            if (isPumpRunning()) 
			{
                waitPumpRunning = new QTimer(this);

                connect(waitPumpRunning, SIGNAL(timeout()), this, SLOT(ensurePumpRunning()));

                waitPumpRunning->start(10 * 1000);
            }
        }
        else 
		{
            if (!isPumpRunning()) 
			{
                ensurePumpStopping();
            }
        }
    }
    else 
	{
        ensurePumpStopping();
    }

	// qDebug() << "onReadyStatusInfo() 2\n"; 
    // freezing
    if (param.getAirTemperature().toInt() <= osConfigInfo.getFreezeTemperature().toInt()) 
	{
        if (pendedCheckFrozen) 
		{
            temp.setWarnFrozen(QString::number(LedInfo::LED_RED));
        }

        processCommand("BBB", "E-STOP");

        freezingAlarm = true;
    }
    else 
	{
        temp.setWarnFrozen(QString::number(LedInfo::LED_OFF));

        if (freezingAlarm) 
		{
            processCommand("BBB", "R-ALARM");

            freezingAlarm = false;
        }
    }

	// qDebug() << "onReadyStatusInfo() 3\n"; 
    // cloudy
    int temperatureGap = qAbs(param.getInstalledTemperature().toInt() - param.getAirTemperature().toInt());

    if ( (temperatureGap <= osConfigInfo.getCloudyTemperature().toInt()) 
		  || (param.getInstalledTemperature().toInt() < param.getAirTemperature().toInt()) ) 
	{
		temp.setCloudy(QString::number(LedInfo::LED_RED));

        processCommand("BBB", "E-STOP");

        cloudyAlarm = true;
    }
    else 
	{
        temp.setCloudy(QString::number(LedInfo::LED_OFF));

        if (cloudyAlarm) 
		{
            processCommand("BBB", "R-ALARM");

            cloudyAlarm = false;
        }
    }

	// qDebug() << "onReadyStatusInfo() 4\n"; 
    // filter pressure
    if (param.getFilterPressure().toInt() >= osConfigInfo.getFilterPressureHigh().toInt()) 
	{
        temp.setFilterPressureSensor(QString::number(LedInfo::LED_RED));

        processCommand("BBB", "E-STOP");
    }
    else 
	{
        temp.setFilterPressureSensor(QString::number(LedInfo::LED_GREEN));
    }

	// qDebug() << "onReadyStatusInfo() 5\n"; 
    // pump1
    if (osConfigInfo.getPumpNumber().split("|")[0] == "1") 
	{
        if (temp.getPump1PressureSensor() == QString::number(LedInfo::LED_OFF))
		{
            temp.setPump1PressureSensor(QString::number(LedInfo::LED_GREEN));
        }

        if (temp.getPump1CtModule() == QString::number(LedInfo::LED_OFF)) 
		{
            temp.setPump1CtModule(QString::number(LedInfo::LED_GREEN));
        }

        if (pumpRunning) 
		{
            // pressure
            if (param.getPump1Pressure().toInt() <= osConfigInfo.getPump1PressureLow().toInt()) 
			{
                temp.setPump1PressureSensor(QString::number(LedInfo::LED_ORANGE));
#if (BOPTS_DISABLE_E_STOP_AFTER_PRESSURE_CHECK == 0)
                processCommand("BBB", "E-STOP");
#endif
            }
            else if (param.getPump1Pressure().toInt() >= osConfigInfo.getPump1PressureHigh().toInt()) 
			{
                temp.setPump1PressureSensor(QString::number(LedInfo::LED_RED));
#if (BOPTS_DISABLE_E_STOP_AFTER_PRESSURE_CHECK == 0)
                processCommand("BBB", "E-STOP");
#endif
            }
            else 
			{
                temp.setPump1PressureSensor(QString::number(LedInfo::LED_GREEN));
            }

            // ct
            if (param.getPump1Ct().toInt() <= osConfigInfo.getPump1CurrentShort().toInt()) 
			{
                temp.setPump1CtModule(QString::number(LedInfo::LED_ORANGE));

                processCommand("BBB", "E-STOP");
            }
            else if (param.getPump1Ct().toInt() >= osConfigInfo.getPump1CurrentOver().toInt())
			{
                temp.setPump1CtModule(QString::number(LedInfo::LED_RED));

                processCommand("BBB", "E-STOP");
            }
            else 
			{
                temp.setPump1CtModule(QString::number(LedInfo::LED_GREEN));
            }
        }
        else 
		{
            if (!isEmergency()) 
			{
                temp.setPump1PressureSensor(QString::number(LedInfo::LED_GREEN));
                temp.setPump1CtModule(QString::number(LedInfo::LED_GREEN));
            }
        }
    }
    else 
	{
        temp.setPump1PressureSensor(QString::number(LedInfo::LED_OFF));
        temp.setPump1CtModule(QString::number(LedInfo::LED_OFF));
    }

	// qDebug() << "onReadyStatusInfo() 6\n"; 
    // pump2
    if (osConfigInfo.getPumpNumber().split("|")[1] == "1") 
	{
        if (temp.getPump2PressureSensor() == QString::number(LedInfo::LED_OFF)) 
		{
            temp.setPump2PressureSensor(QString::number(LedInfo::LED_GREEN));
        }

        if (temp.getPump2CtModule() == QString::number(LedInfo::LED_OFF)) 
		{
            temp.setPump2CtModule(QString::number(LedInfo::LED_GREEN));
        }

        if (pumpRunning) 
		{
            // pressure
            if (param.getPump2Pressure().toInt() <= osConfigInfo.getPump2PressureLow().toInt()) 
			{
                temp.setPump2PressureSensor(QString::number(LedInfo::LED_ORANGE));
#if (BOPTS_DISABLE_E_STOP_AFTER_PRESSURE_CHECK == 0)
                processCommand("BBB", "E-STOP");
#endif
            }
            else if (param.getPump2Pressure().toInt() >= osConfigInfo.getPump2PressureHigh().toInt()) 
			{
                temp.setPump2PressureSensor(QString::number(LedInfo::LED_RED));
#if (BOPTS_DISABLE_E_STOP_AFTER_PRESSURE_CHECK == 0)
                processCommand("BBB", "E-STOP");
#endif
            }
            else 
			{
                temp.setPump2PressureSensor(QString::number(LedInfo::LED_GREEN));
            }

            // ct
            if (param.getPump2Ct().toInt() <= osConfigInfo.getPump2CurrentShort().toInt()) 
			{
                temp.setPump2CtModule(QString::number(LedInfo::LED_ORANGE));

                processCommand("BBB", "E-STOP");
            }
            else if (param.getPump2Ct().toInt() >= osConfigInfo.getPump2CurrentOver().toInt()) 
			{
                temp.setPump2CtModule(QString::number(LedInfo::LED_RED));

                processCommand("BBB", "E-STOP");
            }
            else 
			{
                temp.setPump2CtModule(QString::number(LedInfo::LED_GREEN));
            }
        }
        else 
		{
            if (!isEmergency()) 
			{
                temp.setPump2PressureSensor(QString::number(LedInfo::LED_GREEN));
                temp.setPump2CtModule(QString::number(LedInfo::LED_GREEN));
            }
        }
    }
    else 
	{
        temp.setPump2PressureSensor(QString::number(LedInfo::LED_OFF));
        temp.setPump2CtModule(QString::number(LedInfo::LED_OFF));
    }

	// qDebug() << "onReadyStatusInfo() 7\n"; 
    if (temp.toString() != extract(param).toString()) 
	{
        processCommand("BBB", "LED", temp.toString().toLocal8Bit());
        param = reflect(temp, param);
    }

	if( bUpdated == true )
	{
		statusInfo = param;
		// qDebug() << "2. Web Update : " << before_dateTime.toString("dd-hh:mm:ss");
		emit changedStatusInfo(statusInfo, QDateTime::currentDateTime());
	}

	// DumpStatusInfo(param);
	// qDebug() << "onReadyStatusInfo()--\n"; 
}
#else
void OS::onReadyStatusInfo(StatusInfo param)
{
	// qDebug() << "onReadyStatusInfo()++\n"; 

    ioBoardTimeoutCount = 0;

    OsConfigInfo osConfigInfo = Context::getInstance()->getOsConfigInfo();
    LedInfo temp = extract(param);

	// qDebug() << "onReadyStatusInfo() 1\n"; 
	
    if (!isEmergency()) 
	{
        if (waitPumpRunning == NULL) 
		{
            if (isPumpRunning()) 
			{
                waitPumpRunning = new QTimer(this);

                connect(waitPumpRunning, SIGNAL(timeout()), this, SLOT(ensurePumpRunning()));

                waitPumpRunning->start(10 * 1000);
            }
        }
        else 
		{
            if (!isPumpRunning()) 
			{
                ensurePumpStopping();
            }
        }
    }
    else 
	{
        ensurePumpStopping();
    }

	// qDebug() << "onReadyStatusInfo() 2\n"; 
    // freezing
    if (param.getAirTemperature().toInt() <= osConfigInfo.getFreezeTemperature().toInt()) 
	{
        if (pendedCheckFrozen) 
		{
            temp.setWarnFrozen(QString::number(LedInfo::LED_RED));
        }

        processCommand("BBB", "E-STOP");

        freezingAlarm = true;
    }
    else 
	{
        temp.setWarnFrozen(QString::number(LedInfo::LED_OFF));

        if (freezingAlarm) 
		{
            processCommand("BBB", "R-ALARM");

            freezingAlarm = false;
        }
    }

	// qDebug() << "onReadyStatusInfo() 3\n"; 
    // cloudy
    int temperatureGap = qAbs(param.getInstalledTemperature().toInt() - param.getAirTemperature().toInt());

    if ( (temperatureGap <= osConfigInfo.getCloudyTemperature().toInt()) 
		  || (param.getInstalledTemperature().toInt() < param.getAirTemperature().toInt()) ) 
	{
		temp.setCloudy(QString::number(LedInfo::LED_RED));

        processCommand("BBB", "E-STOP");

        cloudyAlarm = true;
    }
    else 
	{
        temp.setCloudy(QString::number(LedInfo::LED_OFF));

        if (cloudyAlarm) 
		{
            processCommand("BBB", "R-ALARM");

            cloudyAlarm = false;
        }
    }

	// qDebug() << "onReadyStatusInfo() 4\n"; 
    // filter pressure
    if (param.getFilterPressure().toInt() >= osConfigInfo.getFilterPressureHigh().toInt()) 
	{
        temp.setFilterPressureSensor(QString::number(LedInfo::LED_RED));

        processCommand("BBB", "E-STOP");
    }
    else 
	{
        temp.setFilterPressureSensor(QString::number(LedInfo::LED_GREEN));
    }

	// qDebug() << "onReadyStatusInfo() 5\n"; 
    // pump1
    if (osConfigInfo.getPumpNumber().split("|")[0] == "1") 
	{
        if (temp.getPump1PressureSensor() == QString::number(LedInfo::LED_OFF)) 
		{
            temp.setPump1PressureSensor(QString::number(LedInfo::LED_GREEN));
        }

        if (temp.getPump1CtModule() == QString::number(LedInfo::LED_OFF)) 
		{
            temp.setPump1CtModule(QString::number(LedInfo::LED_GREEN));
        }

        if (pumpRunning) 
		{
            // pressure
            if (param.getPump1Pressure().toInt() <= osConfigInfo.getPump1PressureLow().toInt()) 
			{
                temp.setPump1PressureSensor(QString::number(LedInfo::LED_ORANGE));

                processCommand("BBB", "E-STOP");
            }
            else if (param.getPump1Pressure().toInt() >= osConfigInfo.getPump1PressureHigh().toInt()) 
			{
                temp.setPump1PressureSensor(QString::number(LedInfo::LED_RED));

                processCommand("BBB", "E-STOP");
            }
            else 
			{
                temp.setPump1PressureSensor(QString::number(LedInfo::LED_GREEN));
            }

            // ct
            if (param.getPump1Ct().toInt() <= osConfigInfo.getPump1CurrentShort().toInt()) 
			{
                temp.setPump1CtModule(QString::number(LedInfo::LED_ORANGE));

                processCommand("BBB", "E-STOP");
            }
            else if (param.getPump1Ct().toInt() >= osConfigInfo.getPump1CurrentOver().toInt())
			{
                temp.setPump1CtModule(QString::number(LedInfo::LED_RED));

                processCommand("BBB", "E-STOP");#endif

            }
            else 
			{
                temp.setPump1CtModule(QString::number(LedInfo::LED_GREEN));
            }
        }
        else 
		{
            if (!isEmergency()) 
			{
                temp.setPump1PressureSensor(QString::number(LedInfo::LED_GREEN));
                temp.setPump1CtModule(QString::number(LedInfo::LED_GREEN));
            }
        }
    }
    else 
	{
        temp.setPump1PressureSensor(QString::number(LedInfo::LED_OFF));
        temp.setPump1CtModule(QString::number(LedInfo::LED_OFF));
    }

	// qDebug() << "onReadyStatusInfo() 6\n"; 
    // pump2
    if (osConfigInfo.getPumpNumber().split("|")[1] == "1") 
	{
        if (temp.getPump2PressureSensor() == QString::number(LedInfo::LED_OFF)) 
		{
            temp.setPump2PressureSensor(QString::number(LedInfo::LED_GREEN));
        }

        if (temp.getPump2CtModule() == QString::number(LedInfo::LED_OFF)) 
		{
            temp.setPump2CtModule(QString::number(LedInfo::LED_GREEN));
        }

        if (pumpRunning) 
		{
            // pressure
            if (param.getPump2Pressure().toInt() <= osConfigInfo.getPump2PressureLow().toInt()) 
			{
                temp.setPump2PressureSensor(QString::number(LedInfo::LED_ORANGE));
                processCommand("BBB", "E-STOP");
            }
            else if (param.getPump2Pressure().toInt() >= osConfigInfo.getPump2PressureHigh().toInt()) 
			{
                temp.setPump2PressureSensor(QString::number(LedInfo::LED_RED));
                processCommand("BBB", "E-STOP");
            }
            else 
			{
                temp.setPump2PressureSensor(QString::number(LedInfo::LED_GREEN));
            }

            // ct
            if (param.getPump2Ct().toInt() <= osConfigInfo.getPump2CurrentShort().toInt()) 
			{
                temp.setPump2CtModule(QString::number(LedInfo::LED_ORANGE));
                processCommand("BBB", "E-STOP");
            }
            else if (param.getPump2Ct().toInt() >= osConfigInfo.getPump2CurrentOver().toInt()) 
			{
                temp.setPump2CtModule(QString::number(LedInfo::LED_RED));
                processCommand("BBB", "E-STOP");
            }
            else 
			{
                temp.setPump2CtModule(QString::number(LedInfo::LED_GREEN));
            }
        }
        else 
		{
            if (!isEmergency()) 
			{
                temp.setPump2PressureSensor(QString::number(LedInfo::LED_GREEN));
                temp.setPump2CtModule(QString::number(LedInfo::LED_GREEN));
            }
        }
    }
    else 
	{
        temp.setPump2PressureSensor(QString::number(LedInfo::LED_OFF));
        temp.setPump2CtModule(QString::number(LedInfo::LED_OFF));
    }

	// qDebug() << "onReadyStatusInfo() 7\n"; 
    if (temp.toString() != extract(param).toString()) 
	{
        processCommand("BBB", "LED", temp.toString().toLocal8Bit());

        param = reflect(temp, param);
    }

	// qDebug() << "onReadyStatusInfo() 8\n"; 
#if 1
	QString tempTime = QDateTime::currentDateTime().toString("hh");
    if( tempTime.toInt() > 22 || tempTime.toInt() < 5 )  //   if 22 hour ~ 4 hour, then save status info per ten minute 
	{
		if( compareStatusInfo(param) || ( before_dateTime.secsTo(QDateTime::currentDateTime()) > 60*10 ) ) 
		{
			qDebug() << "1. Web Update !!!";
			before_dateTime = QDateTime::currentDateTime();
			statusInfo = param;
			emit changedStatusInfo(statusInfo, QDateTime::currentDateTime());
		}
    } 
	else if( compareStatusInfo(param) || ( before_dateTime.secsTo(QDateTime::currentDateTime()) > 60*5 ) ) 
	{
		qDebug() << "2. Web Update !!!";
		
	    before_dateTime = QDateTime::currentDateTime();
		statusInfo = param;
		emit changedStatusInfo(statusInfo, QDateTime::currentDateTime());
	}
#else
        if (param.toString() != statusInfo.toString()) {
            statusInfo = param;
	
			emit changedStatusInfo(statusInfo, QDateTime::currentDateTime());
		}
#endif

	// qDebug() << "onReadyStatusInfo()--\n"; 
}
#endif // 

void OS::onIoTimeout()
{
    //qDebug() << "onIoTimeout()++\n";
	
    ioBoardTimeoutCount++;

//    if (ioBoardTimeoutCount == 5) {
//        resetIoBoard();
//    }

    StatusInfo capturedStatus = statusInfo;
    LedInfo temp = extract(capturedStatus);

    temp.setIoBoard("0");

    if (temp.toString() != extract(capturedStatus).toString()) 
	{
        capturedStatus = reflect(temp, capturedStatus);
        processCommand("BBB", "LED", temp.toString().toLocal8Bit());
    }

	if (capturedStatus.toString() != statusInfo.toString()) 
	{
		statusInfo = capturedStatus;

		emit changedStatusInfo(statusInfo, QDateTime::currentDateTime());
	}

    //qDebug() << "onIoTimeout()--\n";
}

void OS::onWsTimein()
{
    StatusInfo capturedStatus = statusInfo;
    LedInfo temp = extract(capturedStatus);

    //qDebug() << "onWsTimein()++\n";

    temp.setWebServer(QString::number(LedInfo::LED_GREEN));

    if (temp.toString() != extract(capturedStatus).toString()) 
	{
        capturedStatus = reflect(temp, capturedStatus);

        processCommand("BBB", "LED", temp.toString().toLocal8Bit());
    }

#if 0
	if (capturedStatus.toString() != statusInfo.toString()) {
		statusInfo = capturedStatus;

		emit changedStatusInfo(statusInfo, QDateTime::currentDateTime());
	}
#endif

    //qDebug() << "onWsTimein()--\n";
}

void OS::onWsTimeout()
{
    StatusInfo capturedStatus = statusInfo;
    LedInfo temp = extract(capturedStatus);

    //qDebug() << "onWsTimeout()++\n";

    temp.setWebServer(QString::number(LedInfo::LED_RED));

    if (temp.toString() != extract(capturedStatus).toString()) 
	{
        capturedStatus = reflect(temp, capturedStatus);

        processCommand("BBB", "LED", temp.toString().toLocal8Bit());
    }

	if (capturedStatus.toString() != statusInfo.toString()) 
	{
		statusInfo = capturedStatus;

		emit changedStatusInfo(statusInfo, QDateTime::currentDateTime());
	}

    //qDebug() << "onWsTimeout()--\n";
}

void OS::onReadyInvInfo(InvInfo param)
{
    StatusInfo capturedStatus = statusInfo;
    LedInfo temp = extract(capturedStatus);

    QString nomal = QString::number(LedInfo::LED_GREEN);
    QString error = QString::number(LedInfo::LED_RED);

	// qDebug() << "onReadyInvInfo()++\n"; 

    switch (param.getSlaveId().toUInt()) 
	{
    case 1:
        temp.setInverter1((param.getGroupId() == "0") ? error : nomal);
        break;

    case 2:
        temp.setInverter2((param.getGroupId() == "0") ? error : nomal);
        break;

    case 3:
        temp.setInverter3((param.getGroupId() == "0") ? error : nomal);
        break;

    case 4:
        temp.setInverter4((param.getGroupId() == "0") ? error : nomal);
        break;

    default:
        break;
    }

    if (temp.toString() != extract(capturedStatus).toString()) 
	{
        capturedStatus = reflect(temp, capturedStatus);

        processCommand("BBB", "LED", temp.toString().toLocal8Bit());
    }

	if (capturedStatus.toString() != statusInfo.toString()) 
	{
		statusInfo = capturedStatus;

		emit changedStatusInfo(statusInfo, QDateTime::currentDateTime());
	}

	// qDebug() << "onReadyInvInfo()--\n"; 
}

void OS::processCommand(QByteArray owner, QByteArray command, QByteArray parameter)
{
    QMutexLocker locker(&mutex);

    QDateTime now = QDateTime::currentDateTime();
    QByteArray ask;
	
    ask.append(command);
    ask.append(" by ");
    ask.append(owner);

    //qDebug() << "processCommand() ++";
    //qDebug() << "\nowner: " << owner << ", command: " << command;
    //qDebug() << "parameter:" << parameter;
	//qDebug();

    if (ioController) 
	{
        if (command == "E-STOP") 
		{
            if (isEmergency()) 
			{
                emit ignoredCommand(ask, now, "IGNORE IN EMERGENCY");
            }
            else 
			{
				// qDebug() << "isEmergency() == FALSE";
                ioController->emergencyStop(owner);
            }
        }
        else if (command == "A-SPRAY") 
		{
		#if 1
            if (isEmergency()) 
			{
                emit ignoredCommand("A-SPRAY by BBB", now, "IGNORE IN EMERGENCY");
            }
            else if (isSpraying()) 
			{
                emit ignoredCommand("A-SPRAY by BBB", now, "IGNORE IN SPRAYING");
            }
            else 
			{
                ioController->sprayWaterAutomatically(owner, parameter);
            }
		#else
		    qDebug() << "Auto Spray Command";
			ioController->sprayWaterAutomatically(owner, parameter);
		#endif 
        }
        else if (command == "M-SPRAY") 
		{
            if (isEmergency()) 
			{
                emit ignoredCommand(ask, now, "IGNORE IN EMERGENCY");
            }
            else  if (isSpraying()) 
			{
                emit ignoredCommand(ask, now, "IGNORE IN SPRAYING");
            }
            else 
			{
                ioController->sprayWaterManually(owner);
            }
        }
        else if (command == "DRAIN") 
		{
            if (isEmergency()) 
			{
                emit ignoredCommand(ask, now, "IGNORE IN EMERGENCY");
            }
            else  if (isDraining()) 
			{
                emit ignoredCommand(ask, now, "IGNORE IN DRAINING");
            }
            else 
			{
                ioController->drainWater(owner);
            }
        }
        else if (command == "R-ALARM") 
		{
            if (!isEmergency()) 
			{
				qDebug() << "isEmergency() == FALSE";
                emit ignoredCommand(ask, now, "IGNORE IN NON-EMERGENCY");
            }
            else 
			{
                ioController->releaseAlarm(owner);
            }
        }
        else if (command == "STATUS") 
		{
            ioController->getStatus(owner);
        }
        else if (command == "LED") 
		{
            ioController->changeLedStatusTo(owner, parameter);
        }
        else if (command == "SETUP") 
		{
             qDebug() << "[OS::processCommand] call updateProfileItems()";
            updateProfileItems();
            ioController->setupIo(owner);
        }
        else if (command == "RESTART") 
		{
            // resetIoBoard();
            QProcess::execute("reboot");
        }
        else 
		{

        }
    }

	// qDebug() << "processCommand() --";
}

void OS::ensurePumpRunning()
{
    if (waitPumpRunning) 
	{
        waitPumpRunning->stop();
    }

    pumpRunning = true;
}

void OS::ensurePumpStopping()
{
    //QThread::sleep(1);  kckim_test_del

    pumpRunning = false;

    if (waitPumpRunning) 
	{
        waitPumpRunning->stop();

        waitPumpRunning->deleteLater();

        waitPumpRunning = NULL;
    }
}

void OS::starCheckFrozen()
{
    pendedCheckFrozen = true;

    pendCheckingFrozen->stop();
    pendCheckingFrozen->deleteLater();
    pendCheckingFrozen = NULL;
}

#if 1
bool OS::compareStatusInfo(StatusInfo param)
{
	bool  bReturn = false;
	
	if( param.getSupplyValve().compare(statusInfo.getSupplyValve()) )
	{
		qDebug() << "getSupplyValve";
		bReturn = true;
	}
	
	if( param.getWaterLevelH().compare(statusInfo.getWaterLevelH()) )
	{
		qDebug() << "getWaterLevelH";
		bReturn = true;
	}
	
	if( param.getWaterLevelM().compare(statusInfo.getWaterLevelM()) )
	{
		qDebug() << "getWaterLevelM";
		bReturn = true;
	}

	if( param.getWaterLevelL().compare(statusInfo.getWaterLevelL()) )
	{
		qDebug() << "getWaterLevelL";
		bReturn = true;
	}

	if( param.getSaltLevelL().compare(statusInfo.getSaltLevelL()) )
	{
		qDebug() << "getSaltLevelL";
		bReturn = true;
	}

	if( param.getIndraftValve().compare(statusInfo.getIndraftValve()) )
	{
		qDebug() << "getIndraftValve";
		bReturn = true;
	}

	if( param.getPump1().compare(statusInfo.getPump1()) )
	{
		qDebug() << "getPump1";
		bReturn = true;
	}

	if( param.getPump2().compare(statusInfo.getPump2()) )
	{
		qDebug() << "getPump2";
		bReturn = true;
	}
	
	if( param.getDrainValve().compare(statusInfo.getDrainValve()) )
	{
		qDebug() << "getDrainValve";
		bReturn = true;
	}

	if( param.getSector1Valve().compare(statusInfo.getSector1Valve()) )
	{
		qDebug() << "getSector1Valve";
		bReturn = true;
	}

	if( param.getSector2Valve().compare(statusInfo.getSector2Valve()) )
	{
		qDebug() << "getSector2Valve";
		bReturn = true;
	}

	if( param.getSector3Valve().compare(statusInfo.getSector3Valve()) )
	{
		qDebug() << "getSector3Valve";
		bReturn = true;
	}

	if( param.getSector4Valve().compare(statusInfo.getSector4Valve()) )
	{
		qDebug() << "getSector4Valve";
		bReturn = true;
	}

	if( param.getSector5Valve().compare(statusInfo.getSector5Valve()) )
	{
		qDebug() << "getSector5Valve";
		bReturn = true;
	}

	if( param.getDoorOpen().compare(statusInfo.getDoorOpen()) )
	{
		qDebug() << "getDoorOpen";
		bReturn = true;
	}

	if( param.getOsShutdown().compare(statusInfo.getOsShutdown()) )
	{
		qDebug() << "getOsShutdown";
		bReturn = true;
	}

	if( param.getWaterSpray().compare(statusInfo.getWaterSpray()) )
	{
		qDebug() << "getWaterSpray";
		bReturn = true;
	}

	if( param.getWaterDrain().compare(statusInfo.getWaterDrain()) )
	{
		qDebug() << "getWaterDrain";
		bReturn = true;
	}

	if( param.getWarnWaterLevelL().compare(statusInfo.getWarnWaterLevelL()) )
	{
		qDebug() << "getWarnWaterLevelL";
		bReturn = true;
	}	

	if( param.getWarnSaltLevelL().compare(statusInfo.getWarnSaltLevelL()) )
	{
		qDebug() << "getWarnSaltLevelL";
		bReturn = true;
	}	
	if( param.getWarnFrozen().compare(statusInfo.getWarnFrozen()) )
	{
		qDebug() << "getWarnFrozen";
		bReturn = true;
	}	

	if( param.getCloudy().compare(statusInfo.getCloudy()) )
	{
		qDebug() << "getCloudy";
		bReturn = true;
	}	

	if( param.getIoBoard().compare(statusInfo.getIoBoard()) )
	{
		qDebug() << "getIoBoard";
		bReturn = true;
	}	

	if( param.getInverter1().compare(statusInfo.getInverter1()) )
	{
		qDebug() << "getInverter1";
		bReturn = true;
	}	

	if( param.getInverter2().compare(statusInfo.getInverter2()) )
	{
		qDebug() << "getInverter2";
		bReturn = true;
	}	

	if( param.getInverter3().compare(statusInfo.getInverter3()) )
	{
		qDebug() << "getInverter3";
		bReturn = true;
	}	

	if( param.getInverter4().compare(statusInfo.getInverter4()) )
	{
		qDebug() << "getInverter4";
		bReturn = true;
	}	

	if( param.getWebServer().compare(statusInfo.getWebServer()) )
	{
		qDebug() << "getWebServer";
		bReturn = true;
	}	

	if( param.getFilterPressureSensor().compare(statusInfo.getFilterPressureSensor()) )
	{
		qDebug() << "getFilterPressureSensor";
		bReturn = true;
	}	

	if( param.getPump1PressureSensor().compare(statusInfo.getPump1PressureSensor()) )
	{
		qDebug() << "getPump1PressureSensor";
		bReturn = true;
	}	

	if( param.getPump2PressureSensor().compare(statusInfo.getPump2PressureSensor()) )
	{
		qDebug() << "getPump2PressureSensor";
		bReturn = true;
	}	

	if( param.getPump1CtModule().compare(statusInfo.getPump1CtModule()) )
	{
		qDebug() << "getPump1CtModule";
		bReturn = true;
	}	

	if( param.getPump2CtModule().compare(statusInfo.getPump2CtModule()) )
	{
	    qDebug() << "getPump2CtModule";
		bReturn = true;
	}

	return bReturn;
}

#else
bool OS::compareStatusInfo(StatusInfo param)
{
	if( ( param.getSupplyValve().compare(statusInfo.getSupplyValve()) )
		|| ( param.getWaterLevelH().compare(statusInfo.getWaterLevelH()) )
		|| ( param.getWaterLevelM().compare(statusInfo.getWaterLevelM()) )
		|| ( param.getWaterLevelL().compare(statusInfo.getWaterLevelL()) )
		|| ( param.getSaltLevelL().compare(statusInfo.getSaltLevelL()) )
		|| ( param.getIndraftValve().compare(statusInfo.getIndraftValve()) )
		|| ( param.getPump1().compare(statusInfo.getPump1()) )
		|| ( param.getPump2().compare(statusInfo.getPump2()) )
		|| ( param.getDrainValve().compare(statusInfo.getDrainValve()) )
		|| ( param.getSector1Valve().compare(statusInfo.getSector1Valve()) )
		|| ( param.getSector2Valve().compare(statusInfo.getSector2Valve()) )
		|| ( param.getSector3Valve().compare(statusInfo.getSector3Valve()) )
		|| ( param.getSector4Valve().compare(statusInfo.getSector4Valve()) )
		|| ( param.getSector5Valve().compare(statusInfo.getSector5Valve()) )
		|| ( param.getDoorOpen().compare(statusInfo.getDoorOpen()) )
		|| ( param.getOsShutdown().compare(statusInfo.getOsShutdown()) )
		|| ( param.getWaterSpray().compare(statusInfo.getWaterSpray()) )
		|| ( param.getWaterDrain().compare(statusInfo.getWaterDrain()) )
		|| ( param.getWarnWaterLevelL().compare(statusInfo.getWarnWaterLevelL()) )
		|| ( param.getWarnSaltLevelL().compare(statusInfo.getWarnSaltLevelL()) )
		|| ( param.getWarnFrozen().compare(statusInfo.getWarnFrozen()) )
		|| ( param.getCloudy().compare(statusInfo.getCloudy()) )
		|| ( param.getIoBoard().compare(statusInfo.getIoBoard()) )
		|| ( param.getInverter1().compare(statusInfo.getInverter1()) )
		|| ( param.getInverter2().compare(statusInfo.getInverter2()) )
		|| ( param.getInverter3().compare(statusInfo.getInverter3()) )
		|| ( param.getInverter4().compare(statusInfo.getInverter4()) )
		|| ( param.getWebServer().compare(statusInfo.getWebServer()) )
		|| ( param.getFilterPressureSensor().compare(statusInfo.getFilterPressureSensor()) )
		|| ( param.getPump1PressureSensor().compare(statusInfo.getPump1PressureSensor()) )
		|| ( param.getPump2PressureSensor().compare(statusInfo.getPump2PressureSensor()) )
		|| ( param.getPump1CtModule().compare(statusInfo.getPump1CtModule()) )
		|| ( param.getPump2CtModule().compare(statusInfo.getPump2CtModule()) )
	){
	    return true;
	}

	return false;
}
#endif // 
