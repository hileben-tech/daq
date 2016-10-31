#include "buildopts.h"
#include "invdatabase.h"
#include "context.h"
#include "osconfiginfo.h"
#include "utility.h"

#include <QMutexLocker>
#include <QDir>
#include <QDate>

#include <QDebug>

//static
const QString InvDatabase::DB_FILE_PATH = "/var/hileben/pvccs/db";

//static
const QString InvDatabase::DB_FILE_NAME = "hileben-pvccs-inv.sqlite";

InvDatabase::InvDatabase(QObject *parent) :
    QThread(parent)
{
    database = QSqlDatabase::addDatabase("QSQLITE", "inv");

    start();

    evacuateDatabase();

    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(evacuateDatabase()));

    timer->start(1 * 60 * 1000); // 5 minutes
}

InvDatabase::~InvDatabase()
{
    closeDB();
}

bool InvDatabase::checkPath(QString path)
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

bool InvDatabase::openDB(QString dbName)
{
    if (!checkPath(DB_FILE_PATH)) {
        return false;
    }

    QString filename(DB_FILE_PATH);
    filename.append(QDir::separator());
    filename.append(dbName);

    database.setDatabaseName(filename);

    return database.open();
}

void InvDatabase::closeDB()
{
    if (database.isOpen()) {
        database.close();
    }
}

bool InvDatabase::checkTable()
{
    bool ret = false;

    if (database.isOpen()) {
        QSqlQuery query(database);

        ret = query.exec("CREATE TABLE IF NOT EXISTS inv ("
                         "id INTEGER PRIMARY KEY, "
                         "plantCode TEXT, "
                         "timestamp TEXT, "
                         "groupId TEXT, "
                         "slaveId TEXT, "
                         "currentLoad TEXT, "
                         "accmPowerDaily TEXT, "
                         "accmPowerTotal TEXT, "
                         "pvVoltage TEXT, "
                         "pvCurrent TEXT, "
                         "pvPower TEXT, "
                         "tRadiation TEXT, "
                         "hRadiation TEXT, "
                         "nearTemperture TEXT, "
                         "moduleTemperture TEXT, "
                         "status TEXT"
                         ")");
    }

    return ret;
}

#if 1
void InvDatabase::process(QString element)
{
	// qDebug() << "InvDatabase::process()++";

    QString parameter = element.split("/")[0].toLatin1();//toAscii();
    QString action = element.split("/")[1].toLatin1();//toAscii();
   
    if (action == "INSERT") 
	{
        emit insertedInvInfo(element, QDateTime::currentDateTime(), "INVERTER INFO. INSERTED");
        emit readyParameter(parameter.toLocal8Bit());
    }
    else if (action == "DELETE") 
	{
        emit deletedInvInfo(element, QDateTime::currentDateTime(), "INVERTER INFO. DELETED");
    }
    else if (action == "SELECT") 
	{
        emit selectedInvInfo("", QDateTime::currentDateTime(), "INVERTER INFO. SELECTED");
    }
   
	// qDebug() << "InvDatabase::process()--";

}
#else
void InvDatabase::process(QString element)
{
    bool ret = database.isOpen();

    if (!ret) 
	{
        ret = openDB(DB_FILE_NAME);
    }

	qDebug() << "InvDatabase::process()++";

    if (ret) 
	{
        checkTable();

        QString parameter = element.split("/")[0].toAscii();
        QString action = element.split("/")[1].toAscii();
        QString statement;

        QSqlQuery sqlQuery(database);

        if (ret == true) {
            if (action == "INSERT") {
                statement = makeInsertionStatementFromParameter(parameter);
            }
            else if (action == "DELETE") {
                statement = makeDeletionStatementFromParameter(parameter);
            }
            else if (action == "SELECT") {
                statement = makeSelectionStatement();
            }
        }

        ret = sqlQuery.exec(statement);

        if (ret == true) {
            if (action == "INSERT") {
                emit insertedInvInfo(element, QDateTime::currentDateTime(), "INVERTER INFO. INSERTED");
                emit readyParameter(parameter.toLocal8Bit());
            }
            else if (action == "DELETE") {
                emit deletedInvInfo(element, QDateTime::currentDateTime(), "INVERTER INFO. DELETED");
            }
            else if (action == "SELECT") {
                emit selectedInvInfo("", QDateTime::currentDateTime(), "INVERTER INFO. SELECTED");

                OsConfigInfo osConfigInfo = Context::getInstance()->getOsConfigInfo();

                while (sqlQuery.next()) {
                    parameter = makeParameterFromQuery(sqlQuery);

                    QStringList temp = parameter.split(',');

                    if (temp[0] == osConfigInfo.getPlantCode() && temp[2] != "0") {
                        emit readyParameter(parameter.toLocal8Bit());
                    }
                    else {
                        deleteInvInfo(temp[1]);
                    }

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
                    Utility::msleep(25);
#else
                    QThread::msleep(25);
#endif
                }
            }
        }

        //closeDB();
    }

	qDebug() << "InvDatabase::process()--";
}
#endif // 

void InvDatabase::addElement(QString element)
{
    QMutexLocker locker(&mutex);

    elementQ.enqueue(element);

    elementAdded.wakeOne();
}

QByteArray InvDatabase::makeParameterFromQuery(QSqlQuery& quey)
{
    QStringList elementList;

    elementList.append(quey.value(1).toString());
    elementList.append(quey.value(2).toString());
    elementList.append(quey.value(3).toString());
    elementList.append(quey.value(4).toString());
    elementList.append(quey.value(5).toString());
    elementList.append(quey.value(6).toString());
    elementList.append(quey.value(7).toString());
    elementList.append(quey.value(8).toString());
    elementList.append(quey.value(9).toString());
    elementList.append(quey.value(10).toString());
    elementList.append(quey.value(11).toString());
    elementList.append(quey.value(12).toString());
    elementList.append(quey.value(13).toString());
    elementList.append(quey.value(14).toString());
    elementList.append(quey.value(15).toString());

    return elementList.join(",").toLocal8Bit();
}

QString InvDatabase::makeInsertionStatementFromParameter(QString& parameter)
{
    QStringList fields = parameter.split(',');
    QString statement = QString("INSERT INTO inv VALUES(NULL, '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10', "
                                "'%11', '%12', '%13', '%14', '%15'"
                                ")")
            .arg(fields[0])
            .arg(fields[1])
            .arg(fields[2])
            .arg(fields[3])
            .arg(fields[4])
            .arg(fields[5])
            .arg(fields[6])
            .arg(fields[7])
            .arg(fields[8])
            .arg(fields[9])
            .arg(fields[10])
            .arg(fields[11])
            .arg(fields[12])
            .arg(fields[13])
            .arg(fields[14]);

    return statement;
}

QString InvDatabase::makeDeletionStatementFromParameter(QString& parameter)
{
    QString statement = QString("DELETE FROM inv WHERE timestamp=%1").arg(parameter);

    return statement;
}

QString InvDatabase::makeSelectionStatement()
{
    QString statement = "SELECT * FROM inv";

    return statement;
}

//virtual
void InvDatabase::run()
{
    QString element;

    for(;;) {
        {
            QMutexLocker locker(&mutex);

            if (elementQ.isEmpty()) {
                elementAdded.wait(&mutex);
            }

            element = elementQ.dequeue();
            //qDebug() << "\n\tINV DB Q size: " << elementQ.size();
        }

        process(element);
    }
}

void InvDatabase ::insertInvInfo(InvInfo invInfo, QDateTime dateTime)
{
    OsConfigInfo osConfigInfo = Context::getInstance()->getOsConfigInfo();
    QString plantCode = osConfigInfo.getPlantCode();

    if (plantCode != "None") {
        QString element;

        element.append(plantCode);
        element.append(",");
        element.append(QString::number(dateTime.toMSecsSinceEpoch()));
        element.append(",");
        element.append(invInfo.toString());
        element.append("/INSERT");

        addElement(element);
    }
    else {
        qDebug() << "\n\tnot add element for None of plant code";
    }
}

void InvDatabase ::deleteInvInfo(QString timestamp)
{
    QString element;

    element.append(timestamp);
    element.append("/DELETE");

    addElement(element);
}

void InvDatabase::evacuateDatabase()
{
    QString element = "ALL/SELECT";

    addElement(element);
}
