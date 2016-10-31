#ifndef INVSERPROTOCOL_H
#define INVSERPROTOCOL_H

#include "serport.h"
#include "invinterface.h"

#include <QByteArray>

class InvSerProtocol : public SerPort, public InvInterface
{
    Q_OBJECT
public:
    explicit InvSerProtocol(QObject *parent = 0);

    virtual int readInstantPower(int slaveId, double& var);        // 현재 전력
    virtual int readGeneratedPowerDaily(int slaveId, double& var); // 일일 누적 발전량
    virtual int readGeneratedPowerTotal(int slaveId, double& var); // 총 누적 발전량
    virtual int readPvVoltage(int slaveId, double& var);           // PV 전압
    virtual int readPvCurrent(int slaveId, double& var);           // PV 전류
    virtual int readPvPower(int slaveId, double& var);             // PV 발전량
    virtual int readTRadiation(int slaveId, double& var);          // 경사면 일사량
    virtual int readHRadiation(int slaveId, double& var);          // 수평면 일사량
    virtual int readOutTemperature(int slaveId, double& var);      // 외부 주위 온도
    virtual int readModuleTemperature(int slaveId, double& var);   // 전지 모듈 온도
    virtual int readStatus(int slaveId, QByteArray& var);          // 상태 정보

    virtual bool isOpen();
    virtual void closeProtocol();

    virtual int openProtocol(QString portName, long baudRate, int dataBits, int stopBits, int parity);
    virtual int openProtocol(QString portName, long baudRate);

protected:
    virtual QByteArray deliverMessage(QByteArray param);
    virtual QByteArray checkSum(QByteArray param);
    virtual bool checkFrameValidity(QByteArray param);

signals:

public slots:

};

#endif // INVSERPROTOCOL_H
