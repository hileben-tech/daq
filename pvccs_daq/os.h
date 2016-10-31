#ifndef OS_H
#define OS_H
#include "buildopts.h"
#include "ostime.h"
#include "logger.h"
#include "iodatabase.h"
#include "setterserver.h"
#include "invdatabase.h"
#include "invmediator.h"
#include "iocontroller.h"
#include "webclient.h"
#include "statusinfo.h"
#include "ledinfo.h"
#include "invconfiginfo.h"
#include "sysfsgpio.h"
#include "udpclient.h"
#include "sckcmodbus.h"
#include <QThread>
#include <QMutex>
#include <QMap>
#include <QTime>
#include <QTimer>

#define STATUS_UPDATE_DAY_PERIOD               1   /* Unit : Minute */    
#define STATUS_UPDATE_NIGHT_PERIOD             1   /* Unit : Minute */    


class OS : public QThread
{
    Q_OBJECT

    QMutex mutex;

    Logger* logger;
    SetterServer* setterServer;
    InvDatabase* invDatabase;
    InvMediator* invMediator;
    IODatabase* ioDatabase;
    IOController* ioController;
    WebClient* webClient;

#if (BOPTS_ADD_FUNCTIONS_FOR_DAQ == 1)
    SckCModbus *sckcModbus;
#endif
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    bool m_sendStatusToWS;
    COsTime *m_OsTime;
#else
    /* ======= hmbae ========== */
	CUdpClient*   m_UdpClient;
	/* ======================== */
#endif

    StatusInfo statusInfo;

    QMap<QTime,uint> profileItems;
    int itemIndex;

    bool freezingAlarm;
    bool cloudyAlarm;

    QTimer* waitPumpRunning;
    bool pumpRunning;

    int ioBoardTimeoutCount;

    SysfsGpio *ioBoardReset;
    QTimer* pendCheckingFrozen;
    bool pendedCheckFrozen;

	QDateTime before_dateTime;

#if (BOPTS_FIX_AUTO_SPRAY_NOT_WORK_AFTER_RESETTING_SPRAYTIMETABLE_ERROR == 1)
    bool profileItemsUpdated;
    QMutex mutexProfileItems;
#endif

public:
    explicit OS(QObject *parent = 0);

private:
    bool isEmergency();
    bool isSpraying();
    bool isDraining();
    bool isFreezing();
    bool isCloudy();
    bool isPumpRunning();

    void updateProfileItems();

    void initializeIoBoardReset();
    void resetIoBoard();

    LedInfo extract(StatusInfo param);
    StatusInfo reflect(LedInfo src, StatusInfo dest);
	bool compareStatusInfo(StatusInfo param);

	/* ==== hmbae ================ */
	int  GetUpdateTimeInterval();
	void DumpStatusInfo(StatusInfo Info);
	/* =========================== */

	
protected:
    virtual void run();

signals:
    void ignoredCommand(QByteArray ask, QDateTime dateTime, QString reson);
    void changedStatusInfo(StatusInfo statusInfo, QDateTime dateTime);
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    void logUpdateTimeResult(QString log);
#endif

public slots:
    void onReadyStatusInfo(StatusInfo param);
    void onIoTimeout();
    void onWsTimein();
    void onWsTimeout();
    void onReadyInvInfo(InvInfo param);
    void processCommand(QByteArray owner, QByteArray command, QByteArray parameter = "");
    void ensurePumpRunning();
    void ensurePumpStopping();
    void starCheckFrozen();
    void resetInverterStatus(InvConfigInfo invConfigInfo);

};

#endif // OS_H
