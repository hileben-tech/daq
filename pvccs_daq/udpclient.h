/* ============================================================== */
/* ============================================================== */
/* ============= udpclient.h ==================================== */
/* ============================================================== */
/* ============================================================== */
#ifndef __UDP_CLIENT_H_
#define __UDP_CLIENT_H_

#include "osconfiginfo.h"

#include <QObject>
#include <QUdpSocket>
#include <QAbstractSocket>
#include <QDebug>


#define HTTP_PORT				80						// HTTP server port (Default is 80)
#define SNTP_PORT				123						// SNTP server port (Default is 123)
#define SMTP_PORT				25						// SMTP server port (Default is 25)
#define SSL_PORT				443						// SSL  server port (Default is 443)
#define UPNP_MCAST_PORT			1900					// UPNP multicast port (Default is 1900)
#define UPNP_LOCAL_PORT			55555					// UPNP local port (Default is random)


#define DIFF_SEC_1900_1970	    (2208988800UL)		// number of seconds between 1900 and 1970


typedef struct _UDPCLIENT_HANDLE
{
	QUdpSocket*   socket;
}UDPCLIENT_HANDLE;


typedef struct _sntp_msg{
	unsigned char	li_vn_mode;
	unsigned char	stratum;
	unsigned char	poll;
	unsigned char	precision;
	unsigned int	root_delay;
	unsigned int	root_dispersion;
	unsigned int	reference_identifier;
	unsigned int	reference_timestamp[2];
	unsigned int	originate_timestamp[2];
	unsigned int	receive_timestamp[2];
	unsigned int	transmit_timestamp[2];
} __attribute__ ((packed)) sntp_msg;

class CUdpClient : public QObject
{
    Q_OBJECT
public:
    explicit CUdpClient(QObject* parent = 0);

	bool Init();
	bool Connect(char* pServerIp, int nPort);
	void Disconnect();
	bool IsConnected();
	void Send(char* data, qint64 len);
	void ConnectToTimeServer();
	bool IsTimeUpdated();
	void UpdateTime();

signals:

public slots:
	void OnConnect();
	void OnDisconnect();
	void OnSend(qint64 bytes);
	void OnReceive();
	

private:
	QUdpSocket*   m_pUdpSocket;
	bool          m_bConnected;
	bool          m_bTimeUpdated;

};

#endif // __UDP_CLIENT_H_
