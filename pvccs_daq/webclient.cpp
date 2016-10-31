#include "buildopts.h"
#include "webclient.h"
#include "utility.h"
#include "context.h"
#include "osconfiginfo.h"
#include "ostime.h"
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
#include "sckcmodbus.h"
#endif
#include <QNetworkRequest>
#include <QByteArray>
#include <QStringList>
#include <QDebug>
#include <QSemaphore>
#include <QWaitCondition>

WebClient::WebClient(QObject *parent) :
    QThread(parent)
{
    Context* ctx = Context::getInstance();

    if (ctx->getTestOs()) 
	{
        //domain = "ostest";
        domain = "osjp";
    }
    else 
	{
    #if 0
        domain = "os";
	#else
        //domain = "osjp";
        domain = "ostest";
	#endif // 
    }

    start();

    nam = new QNetworkAccessManager(this);

    connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));
    connect(this, SIGNAL(readyConfigParameter(QByteArray)), this, SLOT(onReadyConfigParameter(QByteArray)));

    ask();
}

void WebClient::ask()
{
    QMutexLocker locker(&mutex);

    if (packetQ.isEmpty()) 
	{
        packetAdded.wait(&mutex);
    }

	//qDebug() << "ask()++";

    QByteArray packet = packetQ.dequeue();

    currpacket = packet;
	
    //qDebug() << "\tWS Q size: " << packetQ.size();

    QNetworkRequest req;
	
	//req.setUrl(QUrl(QString("http://120.142.9.134:8011/dev/index.php")));
    req.setUrl(QUrl(QString("http://%1.hileben.com/dev/index.php").arg(domain)));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    packet.push_front("p=");

    reply = nam->post(req, packet);

    QString temp = packet;
    if(!temp.contains("COMMAND")) 
	{
        //qDebug() << "WEB <<< " << packet;
        emit sentPacket(packet, QDateTime::currentDateTime(), "BBB >> WS");
    }

    // qDebug() << "BBB>>WEB :  " << packet;
    // nam->deleteResource(req);

    timeoutTimer = new QTimer(this);
    connect(timeoutTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    timeoutTimer->setInterval(90 * 1000);   /* ms */
    timeoutTimer->setSingleShot(true);
    timeoutTimer->start();

	//qDebug() << "ask()--";

}

void WebClient::addPacket(QByteArray packet)
{
    QMutexLocker locker(&mutex);

    /*
    packet.replace(QByteArray("^"), QByteArray("%5E"));
	packet.replace(QByteArray(","), QByteArray("%2C"));
	packet.replace(QByteArray("$"), QByteArray("%24"));
	*/

    packetQ.enqueue(packet);

    packetAdded.wakeOne();
}

QByteArray WebClient::makePacket(QString type, QString cmd, QString param)
{
    QByteArray packet;
    packet.append(type);
    packet.append(",");
    packet.append("BBB");
    packet.append(",");
    packet.append(cmd);
    packet.append(",");
    packet.append(param);

    QString checksum = Utility::checkSum(packet);

    packet.push_front("^");
    packet.append("*");
    packet.append(checksum);
    packet.append("$");

    //qDebug() << "makePacket : " << packet;
    return packet;
}

QByteArray WebClient::makeReqPacket(QString cmd, QString param)
{
    //qDebug() << "makeReqPacket : " << cmd;
    return makePacket("REQ", cmd, param);
}

QByteArray WebClient::makeResPacket(QString cmd, QString param)
{
    return makePacket("RES", cmd, param);
}

void WebClient::removePacketAll()
{
    packetQ.clear();
}

//virtual
void WebClient::run()
{
    for (;;) 
	{
        if (packetQ.isEmpty())
		{
            OsConfigInfo osConfigInfo = Context::getInstance()->getOsConfigInfo();

            QStringList parameter;

            parameter.append(osConfigInfo.getPlantCode());
            parameter.append(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));

            addPacket(makeReqPacket("COMMAND", parameter.join(",")));
        }
        else
        {
            //qDebug() << "[WebClinet::run] Que is not Emptry";
        }

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
        Utility::msleep(1000);
#else
        QThread::msleep(1000);
		// QThread::msleep(10000);
#endif
    }
}

void WebClient::onFinished(QNetworkReply *reply)
{
    timeoutTimer->stop();
    timeoutTimer->deleteLater();
    timeoutTimer = 0;

    QByteArray packet;

    //qDebug() << "onFinished()++";

    if(reply->error() == QNetworkReply::NoError) 
	{
        packet = reply->readAll();

		
        QString temp = packet;
        if (!temp.contains("COMMAND")) 
		{
            //qDebug() << "WEB >>> packet = " << packet;
            emit receivedPacket(packet, QDateTime::currentDateTime(), "BBB << WS");
        }

        //qDebug() << "packet = " << packet;
        //qDebug() << "WEB >> PACKET_SIZE= " << packet.size();

        if (packet.size() > 0 && packet.contains("^") && packet.contains("$")) 
		{
            QByteArray frame;
			char* pPacket = packet.data();
            char* pStart;
			char* pEnd;
			int   nSize;
            char* Temp;

			nSize = packet.size();

			Temp = new char[nSize + 1];
			memset(Temp, 0, sizeof(char) * (nSize + 1));

			pStart = strstr(pPacket, "^");
			pEnd   = strstr(pPacket, "$");

			strncpy(Temp, pStart, pEnd-pStart+1);

			frame = Temp;

			delete[] Temp;
			
            frame.replace("^", "");
            frame.replace("$", "");

            QByteArray data = frame.split('*')[0];
            QByteArray sum  = frame.split('*')[1];
			QByteArray checksum = Utility::checkSum(data);

            //qDebug() << "data=" << data;
            //qDebug() << "sum=" << sum << "checksum=" << checksum << "size=" << data.size();

            if ( sum == checksum )
			{
                emit readyPacket(packet, QDateTime::currentDateTime());

                Context* const ctx = Context::getInstance();

                QByteArray type = Utility::getType(data);
                QByteArray owner = Utility::getOwner(data);
                QByteArray command = Utility::getCommand(data);
                QByteArray parameter = Utility::getParameter(data);

            
                if (type == "REQ") 
				{
					//qDebug() << "\n\t   type: " << type;
	                //qDebug() << "\t    owner: " << owner;
	                //qDebug() << "\t  command: " << command;
	                //qDebug() << "\tparameter: " << parameter;
				
                    if (command == "CONFIG") 
					{
                        emit readyConfigParameter(ctx->getOsConfigInfo().toString().toLocal8Bit());
                    }
                    else if(command == "SETUP") 
					{
                        ctx->setOsConfigInfo(Utility::makeOsConfigInfoFromParameter(parameter));

                        emit readyCommand(Utility::getOwner(data), command);
                        emit readyConfigParameter(ctx->getOsConfigInfo().toString().toLocal8Bit());
                    }
                    else if((command == "E-STOP") || (command == "M-SPRAY") 
						     || (command == "DRAIN") || (command == "R-ALARM")) 
					{
                        emit readyCommand(Utility::getOwner(data), command);
                    }
                }
                else /* Response */
				{
                    if (command == "STATUS") 
					{
                        //qDebug() << "[WebClient::onFinished] WS->BBB : STATUS - " << parameter.split(',')[1];
                        QString timestamp = parameter.split(',')[1];

                        emit insertedStatusInfo(timestamp);//parameter.split(',')[1]);

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)

                        /* Check the system time and request to update if it's not correct */
                        if (Utility::isCurTimeValid(timestamp) == false)
                        {
                            //qDebug() << "[WebClient::onFinished] BAD TIME!!!!!!";
                            emit requestTimeUpdate(); /* Request OS time update */
                        }
#endif
                    }
                    else if (command == "INVERTER") 
					{
                        emit insertedInvInfo(parameter.split(',')[1]);
                    }
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
                    else if (command == "COMMAND")
                    {
                        //qDebug() << "[WebClient::onFinished] WS->BBB : COMMAND";
                    }
                    else if (command == "TIME")
                    {
                        //qDebug() << "[WebClient::onFinished] WS->BBB : TIME - " << parameter.split(',')[1];
                        emit receivedWebTime(parameter.split(',')[1]);  /* signal to OsTime */
                    }
                    else
                    {
                        //qDebug() << "[WebClient::onFinished] WS->BBB : Unknown Response(" << command << ")";
                    }
#endif
                }
            }
            else 
			{
				qDebug() << "CheckSum Error";
                emit ignoredPacket(packet, QDateTime::currentDateTime(), "IGNORE INVALID PACKET (as checking sum) @WS");
            }
        }
        else 
		{
            qDebug() << "Invalid Packet from WS";
            emit ignoredPacket(packet, QDateTime::currentDateTime(), "IGNORE INVALID PACKET @WS");
        }
    }
    else 
	{
		QString strError = "IGNORE AS ERROR @WS : Error = ";

	    strError += reply->errorString();

		//if (!currpacket.contains("COMMAND")) 
		//{
		//	addPacket(currpacket);
		//}
        //removePacketAll();
		
        emit ignoredPacket(packet, QDateTime::currentDateTime(), strError);
    }

    reply->deleteLater();

    ask();

    //qDebug() << "onFinished()--";
}

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
void WebClient::sendTimeReqPacket()
{
    if (packetQ.isEmpty())
    {
        QStringList parameter;
        OsConfigInfo osConfigInfo = Context::getInstance()->getOsConfigInfo();

        //qDebug() << "[WebClient::sendTimeReqPacket] ++ ";

        parameter.append(osConfigInfo.getPlantCode());
        parameter.append(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));
        addPacket(makeReqPacket("TIME", parameter.join(",")));

        //qDebug() << "[WebClient::sendTimeReqPacket] --";
    }
    else
    {
        //qDebug() << "[WebClient::sendTimeReqPacket] Que is not Empty.";
    }
}
#endif

#if (BOPTS_ADD_FUNCTIONS_FOR_DAQ == 1)
void WebClient::sendStatusDAQPacket(SckCModbus *sckcModbus)
{
    if (packetQ.isEmpty())
    {
        QStringList parameter;
        OsConfigInfo osConfigInfo = Context::getInstance()->getOsConfigInfo();

        //qDebug() << "[WebClient::sendStatusDAQPacket] ++ ";

        parameter.append(osConfigInfo.getPlantCode());
        parameter.append(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));
        parameter.append(sckcModbus->getModuleName(0));
        parameter.append(sckcModbus->getSerialNum(0));
        parameter.append(sckcModbus->getChannelStatus());
        for (int ch = 1; ch <= 8; ch++)
        {
            if (sckcModbus->isChConnected(ch) == true)
            {
                parameter.append(sckcModbus->getModuleName(ch));
                parameter.append(sckcModbus->getSerialNum(ch));
                parameter.append(sckcModbus->getVoltage(ch));
                parameter.append(sckcModbus->getCurrent(ch));
            }
        }
        addPacket(makeReqPacket("STATUSDAQ", parameter.join(",")));

        qDebug() << "[WebClient::sendStatusDAQPacket] --" << parameter;
    }
    else
    {
        //qDebug() << "[WebClient::sendTimeReqPacket] Que is not Empty.";
    }
}

#endif

void WebClient::onTimeout()
{
    //qDebug() << "WebClient::onTimeout()++";
	
    reply->abort();

	removePacketAll();

    emit timeout();
    emit ignoredPacket("", QDateTime::currentDateTime(), "IGNORE AS TIMEOUT @WS");

    //qDebug() << "WebClient::onTimeout()--";
}

void WebClient::onReadyIoParameter(QByteArray parameter)
{
    //qDebug() << "onReadyIoParameter()++";
	
    addPacket(makeReqPacket("STATUS", parameter));

    //qDebug() << "onReadyIoParameter()--";
}

void WebClient::onReadyInvParameter(QByteArray parameter)
{
    addPacket(makeReqPacket("INVERTER", parameter));
}

void WebClient::onReadyConfigParameter(QByteArray parameter)
{
    addPacket(makeReqPacket("CONFIG", parameter));
}
