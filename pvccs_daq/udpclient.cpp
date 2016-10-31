/* ============================================================== */
/* ============================================================== */
/* ============= udpclient.cpp ================================== */
/* ============================================================== */
/* ============================================================== */


#include "udpclient.h"
#include <time.h>
#include <sys/time.h>
#include <QDateTime>


CUdpClient::CUdpClient(QObject* parent) : QObject(parent)
{
	m_pUdpSocket   = NULL;
    m_bConnected   = false;
    m_bTimeUpdated = false;
}

bool CUdpClient::Init()
{
	m_pUdpSocket = new QUdpSocket(this);

	connect(m_pUdpSocket, SIGNAL(connected()),this, SLOT(OnConnect()));
    connect(m_pUdpSocket, SIGNAL(disconnected()),this, SLOT(OnDisconnect()));
    connect(m_pUdpSocket, SIGNAL(bytesWritten(qint64)),this, SLOT(OnSend(qint64)));
    connect(m_pUdpSocket, SIGNAL(readyRead()),this, SLOT(OnReceive()));

    return true;
}

bool CUdpClient::Connect(char* pServerIp, int nPort)
{
    bool  bReturn = false;
	
    if( m_bConnected == false )
	{
	    qDebug() << "connecting...";

		m_pUdpSocket->connectToHost(pServerIp, nPort);

	    // we need to wait...
	#if 0    
	    if(!m_pUdpSocket->waitForConnected(1000))
	    {
	        qDebug() << "Error: " << m_pUdpSocket->errorString();
	    }
	#endif // 	

        bReturn = true;
	}

	return bReturn;
}

void CUdpClient::Disconnect()
{
}

bool CUdpClient::IsConnected()
{
	return m_bConnected;
}

void CUdpClient::OnConnect()
{
	sntp_msg   msg;

	// memset(&msg, 0, sizeof(msg));
	
    qDebug() << "connected...";

    m_bConnected = true;

	// m_pUdpSocket->write((char*)&msg, sizeof(msg));
}

void CUdpClient::OnDisconnect()
{
    qDebug() << "disconnected...";

    m_bConnected = false;
}

void CUdpClient::OnSend(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void CUdpClient::OnReceive()
{
    qDebug() << "OnReceive() ++";
    
    sntp_msg  msg;
	unsigned long  uTimeStamp;
	qint64   nRecvSize;
	char     buffer[100];
	
    nRecvSize = m_pUdpSocket->read((char*)&msg, sizeof(msg));

	qDebug() << "Receive Size : " << nRecvSize;

	struct tm*  tTime;

#if 0
	qDebug() << "msg.li_vn_mode             = " << msg.li_vn_mode << "\n";
	qDebug() << "msg.stratum                = " << msg.stratum << "\n";
	qDebug() << "msg.poll                   = " << msg.poll << "\n";
	qDebug() << "msg.precision              = " << msg.precision << "\n";
	qDebug() << "msg.root_delay             = " << msg.root_delay << "\n";
	qDebug() << "msg.root_dispersion        = " << msg.root_dispersion << "\n";
	qDebug() << "msg.reference_identifier   = " << msg.reference_identifier;
	qDebug() << "msg.reference_timestamp[0] = " << msg.reference_timestamp[0] << "\n";
	qDebug() << "msg.reference_timestamp[1] = " << msg.reference_timestamp[1] << "\n";
	qDebug() << "msg.originate_timestamp[0] = " << msg.originate_timestamp[0] << "\n";
	qDebug() << "msg.originate_timestamp[1] = " << msg.originate_timestamp[1] << "\n";
	qDebug() << "msg.receive_timestamp[0]   = " << msg.receive_timestamp[0] << "\n";
	qDebug() << "msg.receive_timestamp[1]   = " << msg.receive_timestamp[1] << "\n";
	qDebug() << "msg.transmit_timestamp[0]  = " << msg.transmit_timestamp[0] << "\n";
	qDebug() << "msg.transmit_timestamp[1]  = " << msg.transmit_timestamp[1] << "\n";]
#endif // 		

	uTimeStamp  = (msg.receive_timestamp[0] & 0xFF000000)>>24;
	uTimeStamp |= (msg.receive_timestamp[0] & 0x00FF0000)>>8;
	uTimeStamp |= (msg.receive_timestamp[0] & 0x0000FF00)<<8;
	uTimeStamp |= (msg.receive_timestamp[0] & 0x000000FF)<<24;
	

	// time_t time = (ntohl(msg.receive_timestamp[0]) - DIFF_SEC_1900_1970);
	time_t time = (uTimeStamp - DIFF_SEC_1900_1970);

	tTime = localtime(&time);

#if 0
	qDebug() << "YEAR  :" << (tTime->tm_year + 1900) << "\n";
	qDebug() << "MONTH :" << (tTime->tm_mon + 1) << "\n";
	qDebug() << "DAY   :" << (tTime->tm_mday) << "\n";
	qDebug() << "HOUR  :" << (tTime->tm_hour) << "\n";
	qDebug() << "MINUTE:" << (tTime->tm_min) << "\n";
	qDebug() << "SECOND:" << (tTime->tm_sec) << "\n";
#endif // 	


	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "date -s \"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\"", tTime->tm_year + 1900, tTime->tm_mon + 1,
		             tTime->tm_mday, tTime->tm_hour, tTime->tm_min, tTime->tm_sec);   

	qDebug() << buffer;

	system(buffer);

    m_bTimeUpdated = true;

	qDebug() << "OnReceive() --";
}

void CUdpClient::Send(char* data, qint64 len)
{
    if( m_bConnected == true )
	{
		m_pUdpSocket->write(data, len);
	}
}

void CUdpClient::ConnectToTimeServer()
{
	Connect("time.bora.net", SNTP_PORT);
}

bool CUdpClient::IsTimeUpdated()
{
	return m_bTimeUpdated;
}

void CUdpClient::UpdateTime()
{
	qDebug() << "UpdateTime() ++";
		
	sntp_msg   msg;
	memset(&msg, 0, sizeof(msg));
	msg.li_vn_mode = 27;
	Send((char*)&msg, sizeof(msg));

	qDebug() << "UpdateTime() --";
}



