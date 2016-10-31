#include "buildopts.h"
#include "logger.h"
#include "utility.h"

#include <unistd.h>

#include <QMutexLocker>
#include <QDir>
#include <QDate>
#include <QStringList>
#include <QFileInfoList>

#include <QDebug>

//static
const QString Logger::LOG_FILE_PATH = "/var/hileben/pvccs/log";

//static
const QString Logger::LOG_FILE_NAME = "pvccs-%1.log";

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
const QString Logger::TIME_LOG_FILE_NAME = "pvccs-time.log";
#endif

Logger::Logger(QObject *parent) :
    QThread(parent)
{
    start();
}

Logger::~Logger()
{
    closeFile();

}

bool Logger::checkPath(QString path)
{
    QDir dir(path);

    if (!dir.exists()) {
        QStringList directoryNames = path.split("/");

        foreach (QString directoryName, directoryNames) {
            if (directoryName.isEmpty()) {
                dir.setPath(QDir::separator());

                continue;
            }

            if (dir.path() == QDir::separator()) {
                dir.setPath(dir.path().append(directoryName));
            }
            else {
                dir.setPath(dir.path().append(QDir::separator()).append(directoryName));
            }

            if (!dir.exists()) {
                if (!dir.mkdir(dir.path())) {
                    break;
                }
            }
        }
    }
    else {
        return true;
    }

    dir.setPath(path);

    return dir.exists();
}

bool Logger::openFile(QDateTime dateTime)
{
    if (!checkPath(LOG_FILE_PATH)) {
        return false;
    }

    QString filename(LOG_FILE_PATH);
    filename.append(QDir::separator());
    filename.append(QString(LOG_FILE_NAME).arg(dateTime.toString("yyyy-MM-dd")));

    if (filename == logFile.fileName()) {
        if (logFile.isOpen()) {
            return true;
        }
    }
    else {
        logFile.flush();

        logFile.close();

        logFile.setFileName(filename);
    }

    return logFile.open(QIODevice::Append);
}

void Logger::closeFile()
{
    if (logFile.isOpen())
    {
        logFile.close();
    }

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
    if (timeLogFile.isOpen())
    {
        timeLogFile.close();
    }
#endif
}

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
bool Logger::openTimeFile()
{
    if (!checkPath(LOG_FILE_PATH)) {
        return false;
    }

    QString filename(LOG_FILE_PATH);
    filename.append(QDir::separator());
    filename.append(TIME_LOG_FILE_NAME);

    if (filename == timeLogFile.fileName())
    {
        if (timeLogFile.isOpen())
        {
            return true;
        }
    }
    else
    {
        timeLogFile.flush();
        timeLogFile.close();
        timeLogFile.setFileName(filename);
    }

    return timeLogFile.open(QIODevice::Append);
}
#endif

void Logger::log(QString message)
{
    DiskAmount diskAmount = Utility::getAmountInfo();
    int usingPercent = (int) (((double) diskAmount.available / (double) diskAmount.total) * (double) 100.0);

    if (usingPercent < 20) {
        deleteOldestMonthLogFile();
    }

    bool ok = openFile(extractDateTime(message));

    if (ok) {
        logFile.write(message.toLatin1()/*toAscii()*/, message.size());
        logFile.flush();
    }
}

void Logger::deleteOldestMonthLogFile()
{
    QDir directory(LOG_FILE_PATH);
    QFileInfoList entryInfoList = directory.entryInfoList();
    QString filename;

    for (int i = 0; i < entryInfoList.size(); ++i) {
        QFileInfo entryInfo = entryInfoList.at(i);
        filename = entryInfo.fileName();

        if ((filename.size() - filename.indexOf(".")) < 3) {
            continue;
        }

        break;
    }

    QString command = QString("rm -rf %1/%2*.log")
            .arg(LOG_FILE_PATH)
            .arg(filename.mid(0, filename.lastIndexOf('-')));
    //qDebug() << "\n\tcommand : " << command;

    system(command.toLatin1()/*toAscii()*/);
}

void Logger::addMessage(QString message)
{
    QMutexLocker locker(&mutex);

    messageQ.enqueue(message);

    messageAdded.wakeOne();
}

QString Logger::makeMessage(QByteArray article, QDateTime dateTime, QString comment)
{
    QString message;

    message.append(dateTime.toString("yyyy-MM-dd hh:mm:ss zzz"));
    message.append("  ");
    message.append(article);

    if (comment.size() > 0) {
        message.append(" [");
        message.append(comment);
        message.append("]");
    }

    message.append("\n");

    return message;
}

QDateTime Logger::extractDateTime(QString message)
{
    QStringList packet = message.split(' ');
    QStringList dateTime;

    dateTime.append(packet[0]);
    dateTime.append(packet[1]);
    dateTime.append(packet[2]);

    return QDateTime::fromString(dateTime.join(" "), "yyyy-MM-dd hh:mm:ss zzz");
}

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
void Logger::logTime(QString message)
{
    DiskAmount diskAmount = Utility::getAmountInfo();
    int usingPercent = (int) (((double) diskAmount.available / (double) diskAmount.total) * (double) 100.0);

    if (usingPercent < 20)
    {
        QString command = QString("rm -rf %1/%2*.log")
                .arg(LOG_FILE_PATH)
                .arg(TIME_LOG_FILE_NAME);
        qDebug() << "\n\tcommand : " << command;

        system(command.toLatin1()/*toAscii()*/);
    }

    bool ok = openTimeFile();

    if (ok)
    {
        timeLogFile.write(message.toLatin1()/*toAscii()*/, message.size());
        timeLogFile.flush();
    }
}
#endif

//virtual
void Logger::run()
{
    QString message;

    for(;;) {
        {
            QMutexLocker locker(&mutex);

            if (messageQ.isEmpty()) {
                messageAdded.wait(&mutex);
            }

            message = messageQ.dequeue();
            //qDebug() << "\n\tLOG Q size: " << messageQ.size();
        }

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
        QString msg = message.split('#')[0];
        if (msg == "$$TIME")
        {
            logTime(message.split('#')[1]);
        }
        else
#endif
        log(message);
    }
}

void Logger::logMessage(QByteArray article, QDateTime dateTime, QString comment)
{
    article.replace("\n", "");

    addMessage(makeMessage(article, dateTime, comment));
}

void Logger::logMessage(QString article, QDateTime dateTime, QString comment)
{
    article.replace("\n", "");

    addMessage(makeMessage(article.toLocal8Bit(), dateTime, comment));
}

void Logger::logMessage(StatusInfo statusInfo, QDateTime dateTime)
{
    addMessage(makeMessage(statusInfo.toString().toLocal8Bit(), dateTime, "IO CHANGED"));
}

void Logger::logMessage(InvInfo invInfo, QDateTime dateTime)
{
    addMessage(makeMessage(invInfo.toString().toLocal8Bit(), dateTime, "INVERTER CAPTURED"));
}

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
QString Logger::makeTimeMessage(QString log)
{
    QString message;

    message.append("$$TIME");
    message.append("#");
    message.append(log);
    message.append("\n");

    return message;
}

void Logger::logTimeMessage(QString log)
{
    addMessage(makeTimeMessage(log));
}
#endif
