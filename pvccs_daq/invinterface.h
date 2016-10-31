#ifndef INVINTERFACE_H
#define INVINTERFACE_H

#include <QByteArray>

class InvInterface
{
public:
    InvInterface();

    virtual int readInstantPower(int slaveId, double& var) = 0;        // 현재 전력
    virtual int readGeneratedPowerDaily(int slaveId, double& var) = 0; // 일일 누적 발전량
    virtual int readGeneratedPowerTotal(int slaveId, double& var) = 0; // 총 누적 발전량
    virtual int readPvVoltage(int slaveId, double& var) = 0;           // PV 전압
    virtual int readPvCurrent(int slaveId, double& var) = 0;           // PV 전류
    virtual int readPvPower(int slaveId, double& var) = 0;             // PV 발전량
    virtual int readTRadiation(int slaveId, double& var) = 0;          // 경사면 일사량
    virtual int readHRadiation(int slaveId, double& var) = 0;          // 수평면 일사량
    virtual int readOutTemperature(int slaveId, double& var) = 0;      // 외부 주위 온도
    virtual int readModuleTemperature(int slaveId, double& var) = 0;   // 전지 모듈 온도
    virtual int readStatus(int slaveId, QByteArray& var) = 0;          // 상태 정보

    virtual bool isOpen() = 0;
    virtual void closeProtocol() = 0;

protected:
    virtual QByteArray deliverMessage(QByteArray param) = 0;
    virtual QByteArray checkSum(QByteArray param) = 0;
    virtual bool checkFrameValidity(QByteArray param) = 0;

};

#endif // INVINTERFACE_H
