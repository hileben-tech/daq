#include "buildopts.h"
#include "iocontroller.h"
#include "utility.h"
#include "context.h"
#include "osconfiginfo.h"

#include <string.h>
#include <unistd.h>

#include <QThreadPool>
#include <QStringList>
#include <QDebug>
#include <QThread>

IOController::IOController(QObject *parent) :
    SerPort(parent)
{
    setTimeout(1);
	
    openPort("/dev/ttyO2", 19200);

	// openPort("/dev/ttyO2", 115200);
    // openPort("/dev/ttyS0", 19200);

    QThreadPool* threadPool = QThreadPool::globalInstance();
    threadPool->start(this);
}

void IOController::emergencyStop(QString owner)
{
    QDateTime now = QDateTime::currentDateTime();
    QByteArray packet = Utility::makeReqPacket(owner, "E-STOP", QString::number(now.toMSecsSinceEpoch()));

    addPacket(packet);
}

void IOController::sprayWaterAutomatically(QString owner, QString param)
{
    QDateTime now = QDateTime::currentDateTime();
    QByteArray packet = Utility::makeReqPacket(owner, "A-SPRAY", param);

    addPacket(packet);
}

void IOController::sprayWaterManually(QString owner)
{
    QDateTime now = QDateTime::currentDateTime();
    QByteArray packet = Utility::makeReqPacket(owner, "M-SPRAY", QString::number(now.toMSecsSinceEpoch()));

    addPacket(packet);
}

void IOController::drainWater(QString owner)
{
    QDateTime now = QDateTime::currentDateTime();
    QByteArray packet = Utility::makeReqPacket(owner, "DRAIN", QString::number(now.toMSecsSinceEpoch()));

    addPacket(packet);
}

void IOController::releaseAlarm(QString owner)
{
    QDateTime now = QDateTime::currentDateTime();
    QByteArray packet = Utility::makeReqPacket(owner, "R-ALARM", QString::number(now.toMSecsSinceEpoch()));

    addPacket(packet);
}

void IOController::getStatus(QString owner)
{
    QDateTime now = QDateTime::currentDateTime();

    if (packetQ.isEmpty()) {
        addPacket(Utility::makeReqPacket(owner, "STATUS", QString::number(now.toMSecsSinceEpoch())));
    }
}

void IOController::changeLedStatusTo(QString owner, QString param)
{
    //QDateTime now = QDateTime::currentDateTime();

    addPacket(Utility::makeReqPacket(owner, "LED", param));
}

void IOController::setupIo(QString owner)
{
    OsConfigInfo osConfigInfo = Context::getInstance()->getOsConfigInfo();
    QDateTime now = QDateTime::currentDateTime();
    QStringList param;

    param.append(osConfigInfo.getPumpNumber());
    param.append(osConfigInfo.getSectorNumber());
    param.append(osConfigInfo.getSprayManualTime());
    param.append(osConfigInfo.getTemperatureCalibration());
    param.append(osConfigInfo.getSolarPowerCalibration());

    addPacket(Utility::makeReqPacket(owner, "SETUP", param.join(",")));
}

//virtual
#if 1
void IOController::run()
{
	QMutexLocker locker(&mutex);
	QByteArray   packet;
	QByteArray   temp;
	
    for(;;) 
	{
        if ( packetQ.isEmpty()) 
		{
            packetAdded.wait(&mutex);
        }
		else
		{
        	packet = packetQ.dequeue();
		
            // qDebug() << "Count : " << packetQ.count();
            // qDebug() << "I/O++";
			
			writeBytes(packet);

		    if (!packet.contains("STATUS")) 
			{
                //qDebug() << "\n>>>> " << packet;
		        emit sentPacket(packet, QDateTime::currentDateTime(), "BBB >> IO");
		    }
            //qDebug() << "Count : " << packetQ.count();

			while(1)
			{	
		        for (int i = 0; i < 3; i++) 
				{
		            temp = ask(packet);
					
		            if (isValidPacket(temp)) 
					{
		                break;
		            }
		        }

                // qDebug() << temp;

		        packet = temp;
		        if (isValidPacket(packet)) 
				{
                    // qDebug() << "\n";
                    // qDebug() << "PACKET<PROC>:" << packet;
		            if( processPacket(packet) == true )
		            {
		            	break;
		            }
		        }
		        else 
				{
		            if (packet.contains("@timeout")) 
					{
		                removePacketAll();

		                emit timeout();
		            }
					break;
		        }
			}

            // qDebug() << "I/O--";
		}
    }
}
#elif 0
void IOController::run()
{
	QMutexLocker locker(&mutex);
	QByteArray   packet;
	QByteArray   temp;

    for(;;) 
	{
		// qDebug() << "++";

		if( packetAdded.wait(&mutex, 100) == true )
		{
        	packet = packetQ.dequeue();
		
			qDebug() << "PACKET:" << packet;
			
			writeBytes(packet);

		    if (!packet.contains("STATUS")) 
			{
		        //qDebug() << "\n>>>> " << packet;
		        emit sentPacket(packet, QDateTime::currentDateTime(), "BBB >> IO");
		    }
		}
		
		//else
		{
	        for (int i = 0; i < 3; i++) 
			{
	            temp = ask(packet);

	            if (isValidPacket(temp)) 
				{
	                break;
	            }
	        }

			//qDebug() << temp;

	        packet = temp;
	        if (isValidPacket(packet)) 
			{
				// qDebug() << "\n";
				// qDebug() << "PACKET<PROC>:" << packet;
				
	            if( processPacket(packet) == true )
	            {
	            }
	        }
	        else 
			{
	            if (packet.contains("@timeout")) 
				{
	                removePacketAll();

	                emit timeout();
	            }
	        }
		}
		// qDebug() << "--";
    }
}
#else 
void IOController::run()
{
	QMutexLocker locker(&mutex);
	QByteArray   packet;
	QByteArray   temp;
	
    for(;;) 
	{
        //if ( packetQ.isEmpty()) 
		//{
        //    packetAdded.wait(&mutex);
        //}
		//else
		{
			// QThread::msleep(250);
			// usleep(250 * 1000);
			
			if( packetQ.isEmpty() == false )
			{
	        	packet = packetQ.dequeue();
			
				qDebug() << "PACKET:" << packet;
				
				writeBytes(packet);
  
			    if (!packet.contains("STATUS")) 
				{
			        //qDebug() << "\n>>>> " << packet;
			        emit sentPacket(packet, QDateTime::currentDateTime(), "BBB >> IO");
			    }
			}

				//qDebug() << "while() ++";	
				//while(1)
				//{
			        for (int i = 0; i < 3; i++) 
					{
			            temp = ask(packet);

			            if (isValidPacket(temp)) 
						{
			                break;
			            }
			        }

					//qDebug() << temp;

			        packet = temp;
			        if (isValidPacket(packet)) 
					{
						// qDebug() << "\n";
						// qDebug() << "PACKET<PROC>:" << packet;
						
			            if( processPacket(packet) == true )
			            {
			            	//break;
			            }
			        }
			        else 
					{
			            if (packet.contains("@timeout")) 
						{
			                removePacketAll();

			                emit timeout();
			            }
						//break;
			        }
				//}
			// }
			//qDebug() << "while() --";	
		}
    }
}
#endif // 



#if 1
QByteArray IOController::ask(QByteArray packet1)
{
	// qDebug() << "ask()++";

	QByteArray  RxPacket;

#if 0 //	
    writeBytes(packet);
  
    if (!packet.contains("STATUS")) 
	{
        //qDebug() << "\n>>>> " << packet;

        emit sentPacket(packet, QDateTime::currentDateTime(), "BBB >> IO");
    }
#endif // 	

    RxPacket = readByStxEtx('^', '$');
    // qDebug() << "<<<< " << packet;

    if (isValidPacket(RxPacket)) 
	{
        QByteArray frame = RxPacket;

        frame.replace("^", "");
        frame.replace("$", "");

		QByteArray data;
		QByteArray sum;
		QList<QByteArray> List = frame.split('*');
		if( !List.empty() )
		{
			int nCount = List.count();

			// qDebug() << "ask() : count = " << nCount;
		  
			data = List[0];
			if( nCount > 1 )
			{
        		sum  = List[1];

				QByteArray checksum = Utility::checkSum(data);

		        if (sum != checksum) 
				{
		            RxPacket = "@invalid";
		        }
			}
			else
			{
				RxPacket = "@invalid";
			}
		}
		else
		{
			RxPacket = "@invalid";
		}
    }

    if( !RxPacket.contains("STATUS") ) 
	{
        emit receivedPacket(RxPacket, QDateTime::currentDateTime(), "BBB << IO");
    }

	// qDebug() << "ask()--";

    return RxPacket;
}

#else
QByteArray IOController::ask(QByteArray packet)
{
	// qDebug() << "ask()++";
	
    writeBytes(packet);
  
    if (!packet.contains("STATUS")) 
	{
        //qDebug() << "\n>>>> " << packet;

        emit sentPacket(packet, QDateTime::currentDateTime(), "BBB >> IO");
    }

    packet = readByStxEtx('^', '$');
    // qDebug() << "<<<< " << packet;

    if (isValidPacket(packet)) 
	{
        QByteArray frame = packet;

        frame.replace("^", "");
        frame.replace("$", "");

	#if 1
		QByteArray data;
		QByteArray sum;
		QList<QByteArray> List = frame.split('*');
		if( !List.empty() )
		{
			int nCount = List.count();

			// qDebug() << "ask() : count = " << nCount;
		  
			data = List[0];
			if( nCount > 1 )
			{
        		sum  = List[1];

				QByteArray checksum = Utility::checkSum(data);

		        if (sum != checksum) 
				{
		            packet = "@invalid";
		        }
			}
			else
			{
				packet = "@invalid";
			}
		}
		else
		{
			packet = "@invalid";
		}
	#else
        QByteArray data = frame.split('*')[0];
        QByteArray sum  = frame.split('*')[1];

		qDebug() << "ask() 4";
		
        QByteArray checksum = Utility::checkSum(data);

        if (sum != checksum) 
		{
            packet = "@invalid";
        }
	#endif // 			
    }

    if (!packet.contains("STATUS")) 
	{
        emit receivedPacket(packet, QDateTime::currentDateTime(), "BBB << IO");
    }

	// qDebug() << "ask()--";

    return packet;
}
#endif // 

StatusInfo IOController::makeStatusInfoFromParameter(QByteArray param)
{
    QList<QByteArray> parameters = param.split(',');
    StatusInfo statusInfo;

    statusInfo.setFilterPressure(parameters[0]);
    statusInfo.setPump1Pressure(parameters[1]);
    statusInfo.setPump2Pressure(parameters[2]);
    statusInfo.setPump1Ct(parameters[3]);
    statusInfo.setPump2Ct(parameters[4]);
    statusInfo.setRadiation(parameters[5]);
    statusInfo.setInstalledTemperature(parameters[6]);
    statusInfo.setUninstalledTemperature(parameters[7]);
    statusInfo.setAirTemperature(parameters[8]);

    statusInfo.setSupplyValve(parameters[9]);
    statusInfo.setWaterLevelH(parameters[10]);
    statusInfo.setWaterLevelM(parameters[11]);
    statusInfo.setWaterLevelL(parameters[12]);
    statusInfo.setSaltLevelL(parameters[13]);
    statusInfo.setIndraftValve(parameters[14]); // »Ì¿‘ πÎ∫Í 
	
    statusInfo.setPump1(parameters[15]);
    statusInfo.setPump2(parameters[16]);
	
    statusInfo.setDrainValve(parameters[17]);
    statusInfo.setSector1Valve(parameters[18]);
    statusInfo.setSector2Valve(parameters[19]);
    statusInfo.setSector3Valve(parameters[20]);
    statusInfo.setSector4Valve(parameters[21]);
    statusInfo.setSector5Valve(parameters[22]);
	
    statusInfo.setDoorOpen(parameters[23]);
	
    statusInfo.setOsShutdown(parameters[24]);
	
    statusInfo.setWaterSpray(parameters[25]);
    statusInfo.setWaterDrain(parameters[26]);
    statusInfo.setWarnWaterLevelL(parameters[27]);
    statusInfo.setWarnSaltLevelL(parameters[28]);

    statusInfo.setWarnFrozen(parameters[29]);
    statusInfo.setCloudy(parameters[30]);
    statusInfo.setIoBoard(parameters[31]);
    statusInfo.setInverter1(parameters[32]);
    statusInfo.setInverter2(parameters[33]);
    statusInfo.setInverter3(parameters[34]);
    statusInfo.setInverter4(parameters[35]);
    statusInfo.setWebServer(parameters[36]);
    statusInfo.setFilterPressureSensor(parameters[37]);
    statusInfo.setPump1PressureSensor(parameters[38]);
    statusInfo.setPump2PressureSensor(parameters[39]);
    statusInfo.setPump1CtModule(parameters[40]);
    statusInfo.setPump2CtModule(parameters[41]);
#if (BOPTS_FIX_VERSION_INFO == 1)
    statusInfo.setIOVersion(parameters[42]);
#endif

    return statusInfo;
}

void IOController::addPacket(QByteArray packet)
{
    QMutexLocker locker(&mutex);

	// qDebug() << "addPacket() : packet=" << packet;

    packetQ.enqueue(packet);

    packetAdded.wakeOne();

	// qDebug() << "addPacket() --";
}

bool IOController::isValidPacket(QByteArray packet)
{
    if (packet.contains("@error")) 
	{
        return false;
    }

    if (packet.contains("@timeout")) 
	{
        return false;
    }

    if (packet.contains("@invalid")) 
	{
        return false;
    }

    return true;
}

void IOController::removePacketAll()
{
    packetQ.clear();
}

bool IOController::processPacket(QByteArray packet)
{
	bool  bReturn = false;
	
    packet.replace("^", "");
    packet.replace("$", "");

	// qDebug() << "P.P() ++";

	QList<QByteArray> List;
	int  nCount;

	List = packet.split(',');

	nCount = List.count();
	if( nCount >= 3 )
	{
	    QByteArray data = packet.split('*')[0];
		//qDebug() << data;
		
	    QByteArray type = Utility::getType(data);
		//qDebug() << "Type=" << type;
		
	    QByteArray owner = Utility::getOwner(data);
		//qDebug() << "Owner=" << owner;
		
	    QByteArray command = Utility::getCommand(data);
		
		if( command == "MSG" )
		{
			// qDebug() << "Command == MSG";
			QByteArray message = Utility::getMessage(data);
			qDebug() << message; 
		}
		else
		{
		    QByteArray parameter = Utility::getParameter(data);
			
			// qDebug() << parameter;
			// qDebug() << "CORE_BOARD";

		    QList<QByteArray> parameters = parameter.split(',');

#if (BOPTS_FIX_VERSION_INFO == 1)
            if (parameters.size() == 43)
#else
            if (parameters.size() == 42)
#endif
            {
		        emit readyStatusInfo(makeStatusInfoFromParameter(parameter));
		    }
		    else 
			{
		        qDebug() << "\n\tparameters size: " << parameters.size();
		        qDebug() << "\tparameters size: " << parameters;
		    }

			bReturn = true;
		}
	}
	
	// qDebug() << "P.P()-- : nCount=" << nCount;

	return bReturn;
}
