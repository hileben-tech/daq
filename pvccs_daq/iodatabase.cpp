#include "iodatabase.h"
#include "context.h"
#include "osconfiginfo.h"
#include "utility.h"

#include <QMutexLocker>
#include <QDir>
#include <QDate>
#include <QDebug>

//static
const QString IODatabase::DB_FILE_PATH = "/var/hileben/pvccs/db";

//static
const QString IODatabase::DB_FILE_NAME = "hileben-pvccs-io.sqlite";

IODatabase::IODatabase(QObject *parent) :
    QThread(parent)
{
    database = QSqlDatabase::addDatabase("QSQLITE", "io");

    start();

    evacuateDatabase();

    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(evacuateDatabase()));

    timer->start(/*2 **/ 60 * 1000); // 2 minutes
}

IODatabase::~IODatabase()
{
    closeDB();
}

bool IODatabase::checkPath(QString path)
{
    QDir dir(path);

    if (!dir.exists()) 
	{
        QStringList directoryNames = path.split("/");

        foreach (QString directoryName, directoryNames)
		{
            if (directoryName.isEmpty()) 
			{
                dir.setPath(QDir::separator());

                continue;
            }

            if (dir.path() == QDir::separator()) 
			{
                dir.setPath(dir.path().append(directoryName));
            }
            else 
			{
                dir.setPath(dir.path().append(QDir::separator()).append(directoryName));
            }

            if (!dir.exists()) 
			{
                if (!dir.mkdir(dir.path())) 
				{
                    break;
                }
            }
        }
    }
    else 
	{
        return true;
    }

    dir.setPath(path);

    return dir.exists();
}

bool IODatabase::openDB(QString dbName)
{
    if (!checkPath(DB_FILE_PATH)) 
	{
        return false;
    }

    QString filename(DB_FILE_PATH);
	
    filename.append(QDir::separator());
    filename.append(dbName);

    database.setDatabaseName(filename);

    return database.open();
}

void IODatabase::closeDB()
{
    if (database.isOpen()) 
	{
        database.close();
    }
}

bool IODatabase::checkTable()
{
    bool ret = false;

    if (database.isOpen()) 
	{
        QSqlQuery query(database);

        ret = query.exec("CREATE TABLE IF NOT EXISTS io ("
                         "id INTEGER PRIMARY KEY, "
                         "plantCode TEXT, "
                         "timestamp TEXT, "
                         "filterPressure TEXT, "
                         "pump1Pressure TEXT, "
                         "pump2Pressure TEXT, "
                         "pump1Ct TEXT, "
                         "pump2Ct TEXT, "
                         "radiation TEXT, "
                         "baseTemperature TEXT, "
                         "operTemperature TEXT, "
                         "airTemperature TEXT, "
                         "supplyValve TEXT, "
                         "waterLevelH TEXT, "
                         "waterLevelM TEXT, "
                         "waterLevelL TEXT, "
                         "saltLevelL TEXT, "
                         "indraftValve TEXT, "
                         "pump1 TEXT, "
                         "pump2 TEXT, "
                         "drainValve TEXT, "
                         "sector1Valve TEXT, "
                         "sector2Valve TEXT, "
                         "sector3Valve TEXT, "
                         "sector4Valve TEXT, "
                         "sector5Valve TEXT, "
                         "doorOpen TEXT, "
                         "osShutdown TEXT, "
                         "waterSpray TEXT, "
                         "waterDrain TEXT, "
                         "warnWaterLevelL TEXT, "
                         "warnSaltLevelL TEXT, "
                         "warnFrozen TEXT, "
                         "cloudy TEXT, "
                         "ioBoard TEXT, "
                         "inverter1 TEXT, "
                         "inverter2 TEXT, "
                         "inverter3 TEXT, "
                         "inverter4 TEXT, "
                         "webServer TEXT, "
                         "filterPressureSensor TEXT, "
                         "pump1PressureSensor TEXT, "
                         "pump2PressureSensor TEXT, "
                         "pump1CtModule TEXT, "
                         "pump2CtModule TEXT"
                         ")");
    }

    return ret;
}


#if 1
void IODatabase::process(QString element)
{
    // bool ret = database.isOpen();
	int  i = 0;

	// qDebug() << "process()++";
	
    QString parameter = element.split("/")[0].toLatin1();//toAscii();
    QString action    = element.split("/")[1].toLatin1();//toAscii();
    QString statement;

    // QSqlQuery sqlQuery(database);

    if(1) 
	{
        if (action == "INSERT") 
		{
            //statement = makeInsertionStatementFromParameter(parameter);
			emit insertedStatusInfo(element, QDateTime::currentDateTime(), "STATUS INFO. INSERTED");
			emit readyParameter(parameter.toLocal8Bit());
        }
        else if (action == "DELETE") 
		{
            //statement = makeDeletionStatementFromParameter(parameter);
            emit deletedStatusInfo(element, QDateTime::currentDateTime(), "STATUS INFO. DELETED");
        }
        else if (action == "SELECT") 
		{
            //statement = makeSelectionStatement();
            emit selectedStatusInfo("", QDateTime::currentDateTime(), "STATUS INFO. SELECTED");
        }
    }

	// qDebug() << "process()--";
}
#elif 0
void IODatabase::process(QString element)
{
    bool ret = database.isOpen();
	int  i = 0;

	qDebug() << "process()++";

    if (!ret) 
	{
        ret = openDB(DB_FILE_NAME);
    }

    if(ret) 
	{
        checkTable();

        QString parameter = element.split("/")[0].toAscii();
        QString action    = element.split("/")[1].toAscii();
        QString statement;

        QSqlQuery sqlQuery(database);

        if (ret == true) 
		{
            if (action == "INSERT") 
			{
                statement = makeInsertionStatementFromParameter(parameter);
            }
            else if (action == "DELETE") 
			{
                statement = makeDeletionStatementFromParameter(parameter);
            }
            else if (action == "SELECT") 
			{
                statement = makeSelectionStatement();
            }
        }

        ret = sqlQuery.exec(statement);
        if (ret == true) 
		{
			qDebug() << "DB open success";

            if (action == "INSERT") 
			{
				qDebug() << "DB INSERT";
                emit insertedStatusInfo(element, QDateTime::currentDateTime(), "STATUS INFO. INSERTED");
                emit readyParameter(parameter.toLocal8Bit());
            }
            else if (action == "DELETE") 
			{
				qDebug() << "DB DELETE";
                emit deletedStatusInfo(element, QDateTime::currentDateTime(), "STATUS INFO. DELETED");
            }
            else if (action == "SELECT") 
			{
                emit selectedStatusInfo("", QDateTime::currentDateTime(), "STATUS INFO. SELECTED");
                OsConfigInfo osConfigInfo = Context::getInstance()->getOsConfigInfo();

				i = 0;
                while (sqlQuery.next()) 
				{
                    parameter = makeParameterFromQuery(sqlQuery);
                    QStringList temp = parameter.split(',');

                    if (temp[0] == osConfigInfo.getPlantCode()) 
					{
						qDebug() << "action=SELECT : " << i++;
						// qDebug() << "param=" << parameter.toLocal8Bit();
                        // emit readyParameter(parameter.toLocal8Bit());
                    }
                    else 
					{
                        deleteStatusInfo(temp[1]);
                    }	

                    QThread::msleep(250);
                }
            }
        }
		else
		{
			qDebug() << "DB open error";
		}

        //closeDB();
    }

	qDebug() << "process()--";
}
#else
void IODatabase::process(QString element)
{
    bool ret = database.isOpen();

	qDebug() << "process()++";

    if (!ret) 
	{
        ret = openDB(DB_FILE_NAME);
    }

    if(ret) 
	{
        checkTable();

        QString parameter = element.split("/")[0].toAscii();
        QString action    = element.split("/")[1].toAscii();
        QString statement;

        QSqlQuery sqlQuery(database);

        if (ret == true) 
		{
            if (action == "INSERT") 
			{
                statement = makeInsertionStatementFromParameter(parameter);
            }
            else if (action == "DELETE") 
			{
                statement = makeDeletionStatementFromParameter(parameter);
            }
            else if (action == "SELECT") 
			{
                statement = makeSelectionStatement();
            }
        }

        ret = sqlQuery.exec(statement);
        if (ret == true) 
		{
			qDebug() << "DB open success";

            if (action == "INSERT") 
			{
                emit insertedStatusInfo(element, QDateTime::currentDateTime(), "STATUS INFO. INSERTED");
                emit readyParameter(parameter.toLocal8Bit());
            }
            else if (action == "DELETE") 
			{
                emit deletedStatusInfo(element, QDateTime::currentDateTime(), "STATUS INFO. DELETED");
            }
            else if (action == "SELECT") 
			{
                emit selectedStatusInfo("", QDateTime::currentDateTime(), "STATUS INFO. SELECTED");

                OsConfigInfo osConfigInfo = Context::getInstance()->getOsConfigInfo();

				/* ============================================== */
			#if 0	
				if( sqlQuery.last() )
				{
					parameter = makeParameterFromQuery(sqlQuery);
		            QStringList temp = parameter.split(',');

		            if (temp[0] == osConfigInfo.getPlantCode()) 
					{
						qDebug() << "param=" << parameter.toLocal8Bit();
		                emit readyParameter(parameter.toLocal8Bit());
		            }
		            else 
					{
		                deleteStatusInfo(temp[1]);
		            }
				}
				/* ============================================== */
			#else
				if( sqlQuery.isActive() )
				{
	                while (sqlQuery.next()) 
					{
					#if 1
	                    parameter = makeParameterFromQuery(sqlQuery);
	                    QStringList temp = parameter.split(',');

	                    if (temp[0] == osConfigInfo.getPlantCode()) 
						{
							qDebug() << "param=" << parameter.toLocal8Bit();
	                        emit readyParameter(parameter.toLocal8Bit());
	                    }
	                    else 
						{
	                        deleteStatusInfo(temp[1]);
	                    }
					#endif // 	

	                    QThread::msleep(250);
	                }
				}
				else
				{
					qDebug() << "inactive!!";
				}
			#endif // 	
            }
        }
		else
		{
			qDebug() << "DB open error";
		}

        //closeDB();
    }

	qDebug() << "process()--";
}
#endif // 

void IODatabase::addElement(QString element)
{
    QMutexLocker locker(&mutex);

    elementQ.enqueue(element);

    elementAdded.wakeOne();
}

QByteArray IODatabase::makeParameterFromQuery(QSqlQuery& quey)
{
    OsConfigInfo osConfigInfo = Context::getInstance()->getOsConfigInfo();
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
    elementList.append(quey.value(16).toString());
    elementList.append(quey.value(17).toString());
    elementList.append(quey.value(18).toString());
    elementList.append(quey.value(19).toString());
    elementList.append(quey.value(20).toString());
    elementList.append(quey.value(21).toString());
    elementList.append(quey.value(22).toString());
    elementList.append(quey.value(23).toString());
    elementList.append(quey.value(24).toString());
    elementList.append(quey.value(25).toString());
    elementList.append(quey.value(26).toString());
    elementList.append(quey.value(27).toString());
    elementList.append(quey.value(28).toString());
    elementList.append(quey.value(29).toString());
    elementList.append(quey.value(30).toString());
    elementList.append(quey.value(31).toString());
    elementList.append(quey.value(32).toString());
    elementList.append(quey.value(33).toString());
    elementList.append(quey.value(34).toString());
    elementList.append(quey.value(35).toString());
    elementList.append(quey.value(36).toString());
    elementList.append(quey.value(37).toString());
    elementList.append(quey.value(38).toString());
    elementList.append(quey.value(39).toString());
    elementList.append(quey.value(40).toString());
    elementList.append(quey.value(41).toString());
    elementList.append(quey.value(42).toString());
    elementList.append(quey.value(43).toString());
    elementList.append(quey.value(44).toString());

    return elementList.join(",").toLocal8Bit();
}

QString IODatabase::makeInsertionStatementFromParameter(QString& parameter)
{
    QStringList fields = parameter.split(',');
    QString statement = QString("INSERT INTO io VALUES(NULL, '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10', "
                                "'%11', '%12', '%13', '%14', '%15', '%16', '%17', '%18', '%19', '%20', "
                                "'%21', '%22', '%23', '%24', '%25', '%26', '%27', '%28', '%29', '%30', "
                                "'%31', '%32', '%33', '%34', '%35', '%36', '%37', '%38', '%39', '%40', "
                                "'%41', '%42', '%43', '%44'"
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
            .arg(fields[14])
            .arg(fields[15])
            .arg(fields[16])
            .arg(fields[17])
            .arg(fields[18])
            .arg(fields[19])
            .arg(fields[20])
            .arg(fields[21])
            .arg(fields[22])
            .arg(fields[23])
            .arg(fields[24])
            .arg(fields[25])
            .arg(fields[26])
            .arg(fields[27])
            .arg(fields[28])
            .arg(fields[29])
            .arg(fields[30])
            .arg(fields[31])
            .arg(fields[32])
            .arg(fields[33])
            .arg(fields[34])
            .arg(fields[35])
            .arg(fields[36])
            .arg(fields[37])
            .arg(fields[38])
            .arg(fields[39])
            .arg(fields[40])
            .arg(fields[41])
            .arg(fields[42])
            .arg(fields[43]);

    return statement;
}

QString IODatabase::makeDeletionStatementFromParameter(QString& parameter)
{
    QString statement = QString("DELETE FROM io WHERE timestamp=%1").arg(parameter);

    return statement;
}

QString IODatabase::makeSelectionStatement()
{
    QString statement = "SELECT * FROM io";

    return statement;
}

//virtual
void IODatabase::run()
{
    QString element;

    for(;;) 
	{
        {
            QMutexLocker locker(&mutex);

            if (elementQ.isEmpty()) 
			{
                elementAdded.wait(&mutex);
             }

            element = elementQ.dequeue();
			
            // qDebug() << "\n\tIO DB Q size: " << elementQ.size();
			// qDebug() << element;
        }

        process(element);
    }
}

void IODatabase ::insertStatusInfo(StatusInfo statusInfo, QDateTime dateTime)
{
    OsConfigInfo osConfigInfo = Context::getInstance()->getOsConfigInfo();
    QString plantCode = osConfigInfo.getPlantCode();

	// qDebug() << "insertStatusInfo()++";

    if (plantCode != "None") 
	{
        QString element;

		// qDebug() << "insertStatusInfo() : ADD";

        element.append(plantCode);
        element.append(",");
        element.append(QString::number(dateTime.toMSecsSinceEpoch()));
        element.append(",");
        element.append(statusInfo.toString());
        element.append("/INSERT");

        addElement(element);
    }

	// qDebug() << "insertStatusInfo()--";
}

void IODatabase ::deleteStatusInfo(QString timestamp)
{
    QString element;

	// qDebug() << "DeleteStatusInfo()++";

    element.append(timestamp);
    element.append("/DELETE");

    addElement(element);

	// qDebug() << "DeleteStatusInfo()--";
}

void IODatabase::evacuateDatabase()
{
    QString element = "ALL/SELECT";

    addElement(element);
}
