#include "setterserver.h"
#include "context.h"
#include "utility.h"

#include <QList>
#include <QByteArray>
#include <QDebug>

SetterServer::SetterServer(QObject *parent) :
    QTcpServer(parent)
{
    //qDebug() << "SetterServer()++";
	
    setter = 0;

    connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

    listen(QHostAddress::Any, 2570);

    //qDebug() << "SetterServer()--";
}

void SetterServer::onNewConnection()
{
	//qDebug() << "onNewConnection()++";
	
    if (setter == 0) 
	{
        setter = nextPendingConnection();

        emit connectedSetter("", QDateTime::currentDateTime(), "SETTER CONNECTED");

        connect(setter, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
        connect(setter, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    }

	//qDebug() << "onNewConnection()--";
}

void SetterServer::onDisconnected()
{
	//qDebug() << "onDisconnected()++";
    if (setter != 0) 
	{
        if (setter->isOpen()) 
		{
            setter->close();
            setter->deleteLater();
        }
    }

    emit disconnectedSetter("", QDateTime::currentDateTime(), "SETTER DISCONNECTED");

	//qDebug() << "onDisconnected()--";

    setter = 0;
}

void SetterServer::onReadyRead()
{
	//qDebug() << "onReadyRead()++";
	
    if (setter->canReadLine()) 
	{
        QByteArray packet = setter->readLine();
        emit receivedPacket(packet, QDateTime::currentDateTime(), "BBB << SETTER");

        // qDebug() << "\n\t<<<<" << packet;

        packet.replace("^", "");
        packet.replace("$", "");
        packet.replace("\n", "");

        QByteArray data = packet.split('*')[0];
        QByteArray sum  = packet.split('*')[1];
        QByteArray checksum = Utility::checkSum(data);

        if (checksum == sum) 
		{
            Context* const ctx = Context::getInstance();

            QByteArray type = Utility::getType(data);
            QByteArray owner = Utility::getOwner(data);
            QByteArray command = Utility::getCommand(data);
            QByteArray parameter = Utility::getParameter(data);

			// qDebug() << type;
			// qDebug() << owner;
			// qDebug() << command;
			// qDebug() << parameter;

            if (type == "REQ") 
			{
                if (command == "CONFIG") 
				{
                    packet = Utility::makeResPacket("BBB", command, ctx->getOsConfigInfo().toString());
                }
                else if (command == "SETUP") 
				{
                    ctx->setOsConfigInfo(Utility::makeOsConfigInfoFromParameter(parameter));

                    emit readyCommand(Utility::getOwner(data), command);

                    packet = Utility::makeResPacket("BBB", command, ctx->getOsConfigInfo().toString());

                }
                else if ( (command == "E-STOP") || (command == "M-SPRAY") || (command == "DRAIN") 
					      || (command == "R-ALARM") || (command == "RESTART") ) 
				{
                    emit readyCommand(Utility::getOwner(data), command);

                    packet = Utility::makeResPacket("BBB", command, ctx->getOsConfigInfo().toString());
                }
            }
            else 
			{
                // nothing to do
            }

            packet.append("\n");

            setter->write(packet);
			
            emit sentPacket(packet, QDateTime::currentDateTime(), "BBB >> SETTER");

            // qDebug() << "\n\t>>>>" << packet;
        }
    }
	else
	{
		qDebug() << "CanReadLine() Error";
	}

	//qDebug() << "onReadyRead()--";
}

