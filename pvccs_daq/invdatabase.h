#ifndef INVDATABASE_H
#define INVDATABASE_H

#include "buildopts.h"
#include "invinfo.h"

#include <QThread>
#include <QQueue>
#include <QWaitCondition>
#include <QMutex>
#include <QFile>
#include <QDateTime>
#include <QtSql>
#include <QSqlQuery>

class InvDatabase : public QThread
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
    explicit InvDatabase(QObject *parent = 0);
    ~InvDatabase();

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
    void insertedInvInfo(QString query, QDateTime dateTime, QString status);
    void deletedInvInfo(QString query, QDateTime dateTime, QString status);
    void selectedInvInfo(QString query, QDateTime dateTime, QString status);

    void readyParameter(QByteArray parameter);

public slots:
    void insertInvInfo(InvInfo invInfo, QDateTime dateTime);
    void deleteInvInfo(QString timestamp);

private slots:
    void evacuateDatabase();

};

#endif // INVDATABASE_H
