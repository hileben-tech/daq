#ifndef SMASC250HESERPROTOCOL_H
#define SMASC250HESERPROTOCOL_H

#include "invserprotocol.h"

class SMAsc250heSerProtocol : public InvSerProtocol
{
    Q_OBJECT

    static const char ENQ = 0x05;
    static const char ACK = 0x06;
    static const char EOT = 0x04;

public:
    explicit SMAsc250heSerProtocol(QObject *parent = 0);

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

protected:
    virtual QByteArray deliverMessage(QByteArray param);
    virtual QByteArray checkSum(QByteArray param);
    virtual bool checkFrameValidity(QByteArray param);

private:
    QByteArray makeFrame(int slaveId, int address, int quantity);
    QByteArray readFrame(QByteArray param);

signals:

public slots:

};

#endif // SMASC250HESERPROTOCOL_H
