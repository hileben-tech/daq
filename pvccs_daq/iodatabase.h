#ifndef IODATABASE_H
#define IODATABASE_H

#include "statusinfo.h"

#include <QThread>
#include <QQueue>
#include <QWaitCondition>
#include <QMutex>
#include <QFile>
#include <QDateTime>
#include <QtSql>
#include <QSqlQuery>

class IODatabase : public QThread
{
    Q_OBJECT

    static const QString DB_FILE_PATH;
    static const QString DB_FILE_NAME;

    QQueue<QString> elementQ;

    QWaitCondition elementAdded;
    QMutex mutex;

    QSqlDatabase database;

    QTimer* timer;

public:
    explicit IODatabase(QObject *parent = 0);
    ~IODatabase();

private:
    bool checkPath(QString path);

    bool openDB(QString dbName);
    void closeDB();
    bool checkTable();

    void process(QString element);

    void addElement(QString element);

    QByteArray makeParameterFromQuery(QSqlQuery& quey);

    QString makeInsertionStatementFromParameter(QString& parameter);
    QString makeDeletionStatementFromParameter(QString& parameter);
    QString makeSelectionStatement();

protected:
    virtual void run();

signals:
    void insertedStatusInfo(QString query, QDateTime dateTime, QString status);
    void deletedStatusInfo(QString query, QDateTime dateTime, QString status);
    void selectedStatusInfo(QString query, QDateTime dateTime, QString status);

    void readyParameter(QByteArray parameter);

public slots:
    void insertStatusInfo(StatusInfo statusInfo, QDateTime dateTime);
    void deleteStatusInfo(QString timestamp);

private slots:
    void evacuateDatabase();

};

#endif // IODATABASE_H
