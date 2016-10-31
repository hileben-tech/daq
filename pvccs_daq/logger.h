#ifndef LOGGER_H
#define LOGGER_H
#include "buildopts.h"
#include "statusinfo.h"
#include "invinfo.h"

#include <QThread>
#include <QQueue>
#include <QWaitCondition>
#include <QMutex>
#include <QFile>
#include <QDateTime>

class Logger : public QThread
{
    Q_OBJECT

    static const QString LOG_FILE_PATH;
    static const QString LOG_FILE_NAME;

    QQueue<QString> messageQ;

    QWaitCondition messageAdded;
    QMutex mutex;

    QFile logFile;
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    static const QString TIME_LOG_FILE_NAME;
    QFile timeLogFile;
#endif

public:
    explicit Logger(QObject *parent = 0);
    ~Logger();

private:
    bool checkPath(QString path);

    bool openFile(QDateTime dateTime);
    void closeFile();

    void log(QString message);
    void deleteOldestMonthLogFile();

    void addMessage(QString message);
    QString makeMessage(QByteArray article, QDateTime dateTime, QString comment);

    QDateTime extractDateTime(QString message);

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    bool openTimeFile();
    void logTime(QString message);
    QString makeTimeMessage(QString log);
#endif

protected:
    virtual void run();

signals:

public slots:
    void logMessage(QByteArray article, QDateTime dateTime, QString comment = "");
    void logMessage(QString article, QDateTime dateTime, QString comment = "");
    void logMessage(StatusInfo statusInfo, QDateTime dateTime);
    void logMessage(InvInfo invInfo, QDateTime dateTime);
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    void logTimeMessage(QString log);
#endif
};

#endif // LOGGER_H
