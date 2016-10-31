#include "buildopts.h"
#include "invmediator.h"
#include "context.h"
#include "invbuilder.h"
#include "utility.h"

#include <QDateTime>
#include <QMutexLocker>
#include <QStringList>
#include <QDebug>

InvMediator::InvMediator(QObject *parent) :
    QThread(parent)
{
    invInterface = 0;

    start();
}

InvMediator::~InvMediator()
{
    InvBuilder::destoryInvInterface(invInterface);
}

void InvMediator::setupInverter()
{
    OsConfigInfo osConfigInfo = Context::getInstance()->getOsConfigInfo();
    InvConfigInfo temp = converterOsConfigInfoToInvConfigInfo(osConfigInfo);

    if (temp.toString() != invConfigInfo.toString()) {
        {
            invConfigInfo = temp;

            slaves.clear();
            slaves = invConfigInfo.getSlaves().split("|");

            InvBuilder::destoryInvInterface(invInterface);
            invInterface = InvBuilder::makeInvInterface(invConfigInfo);

            emit changedInvConfigInfo(invConfigInfo);
        }
    }
}

InvConfigInfo InvMediator::converterOsConfigInfoToInvConfigInfo(OsConfigInfo osConfigInfo)
{
    InvConfigInfo temp;

    temp.setMasterType(osConfigInfo.getInverterMasterType());
    temp.setProtocolType(osConfigInfo.getInverterProtocolType());
    temp.setConnectionType(osConfigInfo.getInverterConnectionType());
    temp.setDeviceName(osConfigInfo.getInverterDeviceName());
    temp.setBaudrate(osConfigInfo.getInverterBaudrate());
    temp.setIpAddress(osConfigInfo.getInverterIpAddress());
    temp.setPortNumber(osConfigInfo.getInverterPortNumber());
    temp.setSlaves(osConfigInfo.getInverterSlaves());

    return temp;
}

//virtual
void InvMediator::run()
{
    Context* ctx = Context::getInstance();

    QDateTime now = QDateTime::currentDateTime();
    QDateTime previous = QDateTime::currentDateTime();

    for (;;) {
        setupInverter();

        if (ctx->getTrackInverter()) {
            QStringList ids;

            for (int i = 1; i < 253; i++) {
                double value;
                if (invInterface->readInstantPower(i, value) == 0) {
                    ids.append(QString::number(i));
                }
            }

            if (ids.size() == 0) {
                qDebug() << "\n\tNot Found Inverter";
            }
            else {
                qDebug() << "\n\tFound " << ids.size() << " Inverters. [" << ids.join(", ") << "]";
            }

            exit();
        }

        if (invConfigInfo.getMasterType() == "OS") {
            now = QDateTime::currentDateTime();

            //qDebug() << "mediator working...";

            if ((now.toTime_t() - previous.toTime_t() > 10)) { // default: 60 seconds
                previous = now;

                if (invInterface && invInterface->isOpen()) {
                    //qDebug();
                    //qDebug();

                    foreach (QString slave, slaves) {
                        QString id = slave.split("+")[0];
                        int slaveId = id.toInt();

                        if (slaveId > 0) {
                            InvInfo invInfo;

                            invInfo.setGroupId(QString::number(now.toMSecsSinceEpoch()));
                            invInfo.setSlaveId(id);

                            double value = 0.0;
                            QByteArray status;

                            if (invInfo.getGroupId() != "0" && invInterface->readInstantPower(slaveId, value) == 0) {
                                invInfo.setInstantPower(QString::number(value, 'f', 2));
                            }
                            else {
                                invInfo.setGroupId("0");
                            }

                            if (invInfo.getGroupId() != "0" && invInterface->readGeneratedPowerDaily(slaveId, value) == 0) {
                                invInfo.setGeneratedPowerDaily(QString::number(value, 'f', 2));
                            }
                            else {
                                invInfo.setGroupId("0");
                            }

                            if (invInfo.getGroupId() != "0" && invInterface->readGeneratedPowerTotal(slaveId, value) == 0) {
                                invInfo.setGeneratedPowerTotal(QString::number(value, 'f', 2));
                            }
                            else {
                                invInfo.setGroupId("0");
                            }

                            if (invInfo.getGroupId() != "0" && invInterface->readPvVoltage(slaveId, value) == 0) {
                                invInfo.setPvVoltage(QString::number(value, 'f', 2));
                            }
                            else {
                                invInfo.setGroupId("0");
                            }

                            if (invInfo.getGroupId() != "0" && invInterface->readPvCurrent(slaveId, value) == 0) {
                                invInfo.setPvCurrent(QString::number(value, 'f', 2));
                            }
                            else {
                                invInfo.setGroupId("0");
                            }

                            if (invInfo.getGroupId() != "0" && invInterface->readPvPower(slaveId, value) == 0) {
                                invInfo.setPvPower(QString::number(value, 'f', 2));
                            }
                            else {
                                invInfo.setGroupId("0");
                            }

                            //                                if (invInfo.getGroupId() != "0" && invInterface->readTRadiation(slaveId, value) == 0) {
                            //                                    invInfo.setTRadiation(QString::number(value, 'f', 2));
                            //                                }
                            //                                else {
                            //                                    invInfo.setGroupId("0");
                            //                                }

                            //                                if (invInfo.getGroupId() != "0" && invInterface->readHRadiation(slaveId, value) == 0) {
                            //                                    invInfo.setHRadiation(QString::number(value, 'f', 2));
                            //                                }
                            //                                else {
                            //                                    invInfo.setGroupId("0");
                            //                                }

                            //                                if (invInfo.getGroupId() != "0" && invInterface->readOutTemperature(slaveId, value) == 0) {
                            //                                    invInfo.setOutTemperature(QString::number(value, 'f', 2));
                            //                                }
                            //                                else {
                            //                                    invInfo.setGroupId("0");
                            //                                }

                            //                                if (invInfo.getGroupId() != "0" && invInterface->readModuleTemperature(slaveId, value) == 0) {
                            //                                    invInfo.setModuleTemperature(QString::number(value, 'f', 2));
                            //                                }
                            //                                else {
                            //                                    invInfo.setGroupId("0");
                            //                                }

                            if (invInfo.getGroupId() != "0" && invInterface->readStatus(slaveId, status) == 0) {
                                invInfo.setStatus(status);
                            }
                            else {
                                invInfo.setGroupId("0");
                            }

                            //qDebug() << invInfo.toString();

                            emit readyInvInfo(invInfo, QDateTime::currentDateTime());
                        }
#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
                        Utility::msleep(25);
#else
                        QThread::msleep(25);
#endif
                   }
                }
            }
        }

#if (BOPTS_USE_WEBSERVER_FOR_SYSTEM_TIME == 1)
        Utility::msleep(250);
#else
        QThread::msleep(250);
#endif
    }
}
