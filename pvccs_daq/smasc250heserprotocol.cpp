#include "smasc250heserprotocol.h"

SMAsc250heSerProtocol::SMAsc250heSerProtocol(QObject *parent) :
    InvSerProtocol(parent)
{
}

//virtual
int SMAsc250heSerProtocol::readInstantPower(int slaveId, double& var)
{
    QByteArray frame;

    frame = makeFrame(slaveId, 0x60, 8);
    frame = readFrame(frame);

    if (checkFrameValidity(frame) == false) {
        return (-1);
    }

    bool ok;
    var = (double) frame.mid(19, 4).toInt(&ok, 16);

    var /= 10.0;

    if (ok == false) {
        return (-1);
    }

    return (0);
}

//virtual
int SMAsc250heSerProtocol::readGeneratedPowerDaily(int slaveId, double& var)
{
    QByteArray frame;

    frame = makeFrame(slaveId, 0x60, 8);
    frame = readFrame(frame);

    if (checkFrameValidity(frame) == false) {
        return (-1);
    }

    bool ok;
    var = (double) frame.mid(27, 4).toInt(&ok, 16);

    if (ok == false) {
        return (-1);
    }

    return (0);
}

//virtual
int SMAsc250heSerProtocol::readGeneratedPowerTotal(int slaveId, double& var)
{
    QByteArray frame;

    frame = makeFrame(slaveId, 0x60, 8);
    frame = readFrame(frame);

    if (checkFrameValidity(frame) == false) {
        return (-1);
    }

    QByteArray temp;

    temp.append(frame.mid(15, 4));
    temp.append(frame.mid(11, 4));

    bool ok;
    var = (double) temp.toInt(&ok, 16);

    if (ok == false) {
        return (-1);
    }

    return (0);
}

//virtual
int SMAsc250heSerProtocol::readPvVoltage(int slaveId, double& var)
{
    QByteArray frame;

    frame = makeFrame(slaveId, 0x20, 2);
    frame = readFrame(frame);

    if (checkFrameValidity(frame) == false) {
        return (-1);
    }

    bool ok;
    var = (double) frame.mid(7, 4).toInt(&ok, 16);

    if (ok == false) {
        return (-1);
    }

    return (0);
}

//virtual
int SMAsc250heSerProtocol::readPvCurrent(int slaveId, double& var)
{
    QByteArray frame;

    frame = makeFrame(slaveId, 0x20, 2);
    frame = readFrame(frame);

    if (checkFrameValidity(frame) == false) {
        return (-1);
    }

    bool ok;
    var = (double) frame.mid(11, 4).toInt(&ok, 16);

    if (ok == false) {
        return (-1);
    }

    return (0);
}

//virtual
int SMAsc250heSerProtocol::readPvPower(int slaveId, double& var)
{
    QByteArray frame;

    frame = makeFrame(slaveId, 0x60, 8);
    frame = readFrame(frame);

    if (checkFrameValidity(frame) == false) {
        return (-1);
    }

    bool ok;
    var = (double) frame.mid(7, 4).toInt(&ok, 16);

    var /= 10.0;

    if (ok == false) {
        return (-1);
    }

    return (0);
}

//virtual
int SMAsc250heSerProtocol::readTRadiation(int slaveId, double& var)
{
    QByteArray frame;

    frame = makeFrame(slaveId, 0x70, 4);
    frame = readFrame(frame);

    if (checkFrameValidity(frame) == false) {
        return (-1);
    }

    bool ok;
    var = (double) frame.mid(7, 4).toInt(&ok, 16);

    if (var <= 400.0) {
        var = 0.0;
    }
    else {
        var -= 400.0;
    }

    var *= 1.25;

    if (ok == false) {
        return (-1);
    }

    return (0);
}

//virtual
int SMAsc250heSerProtocol::readHRadiation(int slaveId, double& var)
{
    QByteArray frame;

    frame = makeFrame(slaveId, 0x70, 4);
    frame = readFrame(frame);

    if (checkFrameValidity(frame) == false) {
        return (-1);
    }

    bool ok;
    var = (double) frame.mid(11, 4).toInt(&ok, 16);

    if (var <= 400.0) {
        var = 0.0;
    }
    else {
        var -= 400.0;
    }

    var *= 1.25;

    if (ok == false) {
        return (-1);
    }

    return (0);
}

//virtual
int SMAsc250heSerProtocol::readOutTemperature(int slaveId, double& var)
{
    QByteArray frame;

    frame = makeFrame(slaveId, 0x70, 4);
    frame = readFrame(frame);

    if (checkFrameValidity(frame) == false) {
        return (-1);
    }

    bool ok;
    var = (double) frame.mid(15, 4).toInt(&ok, 16);

    if (var <= 400.0) {
        var = 0.0;
    }
    else {
        var -= 400.0;
    }

    var /= 16.0;
    var -= 20.0;

    if (ok == false) {
        return (-1);
    }

    return (0);
}

//virtual
int SMAsc250heSerProtocol::readModuleTemperature(int slaveId, double& var)
{
    QByteArray frame;

    frame = makeFrame(slaveId, 0x70, 4);
    frame = readFrame(frame);

    if (checkFrameValidity(frame) == false) {
        return (-1);
    }

    bool ok;
    var = (double) frame.mid(19, 4).toInt(&ok, 16);

    if (var <= 400.0) {
        var = 0.0;
    }
    else {
        var -= 400.0;
    }

    var /= 16.0;
    var -= 20.0;

    if (ok == false) {
        return (-1);
    }

    return (0);
}

//virtual
int SMAsc250heSerProtocol::readStatus(int slaveId, QByteArray& var)
{
    QByteArray frame;

    frame = makeFrame(slaveId, 0x4, 4);
    frame = readFrame(frame);

    if (checkFrameValidity(frame) == false) {
        return (-1);
    }

    var.append(frame.mid(7, 4));
    var.append("|");
    var.append(frame.mid(11, 4));
    var.append("|");
    var.append(frame.mid(15, 4));
    var.append("|");
    var.append(frame.mid(19, 4));

    return (0);
}

//virtual
QByteArray SMAsc250heSerProtocol::deliverMessage(QByteArray param)
{
    writeBytes(param);

    QByteArray frame = readByStxEtx(ACK, EOT);

    if (checkFrameValidity(frame) != true) {
        return frame;
    }

    frame.remove((frame.size() - 1), 1);
    frame.remove(0, 1);
    //frame = QByteArray::fromHex(frame);

    QByteArray data = frame.mid(0, (frame.size() - 4));
    QByteArray sum = frame.mid((frame.size() - 4), 4);

    if (checkSum(data) == sum) {
        return data;
    }

    return "@invaliable";
}

//virtual
QByteArray SMAsc250heSerProtocol::checkSum(QByteArray param)
{
    short check = 0;

    for (int i = 0; i < param.size(); i++) {
        check += param.at(i);
    }

    QString sum;
    sum.sprintf("%04x", check);

    return QByteArray(sum.toLatin1());//toAscii());
}

//virtual
bool SMAsc250heSerProtocol::checkFrameValidity(QByteArray param)
{
    if (param.contains("@error") == true) {
        return false;
    }

    if (param.contains("@timeout") == true) {
        return false;
    }

    if (param.contains("@invalid") == true) {
        return false;
    }

    return true;
}

QByteArray SMAsc250heSerProtocol::makeFrame(int slaveId, int address, int quantity)
{
    QByteArray data, frame;
    QString id, cmd, addr, cnt, sum;

    id.sprintf("%02d", slaveId);
    cmd = "R";
    addr.sprintf("%04x", address);
    cnt.sprintf("%02d", quantity);

    data.append(id);
    data.append(cmd);
    data.append(addr);
    data.append(cnt);

    sum = checkSum(data);

    data.append(sum);
    //data = data.toHex();

    frame.append(ENQ);
    frame.append(data);
    frame.append(EOT);

    return frame;
}

QByteArray SMAsc250heSerProtocol::readFrame(QByteArray param)
{
    QByteArray frame;

    for (int i = 0; i < 3; i++) {
        frame = deliverMessage(param);

        if (checkFrameValidity(frame)) {
            break;
        }
    }

    return frame;
}
