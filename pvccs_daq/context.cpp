#include "context.h"
#include "buildopts.h"
#include <QSettings>
#include <QDebug>

//static
Context* const Context::instance = new Context();

Context::Context(QObject *parent) :
    QObject(parent)
{
    useInverterOnly = false;
    trackInverter = false;
    testOs = false;
    useInverter = true;

    qDebug() << "";

    readSettings();
}

//static
Context* Context::getInstance()
{
    return instance;
}

OsConfigInfo Context::getOsConfigInfo()
{
    //readSettings();

    return osConfigInfo;
}

void Context::setOsConfigInfo(OsConfigInfo param)
{
    osConfigInfo = param;

    writeSettings();
}

bool Context::getUseInverterOnly()
{
    return useInverterOnly;
}

void Context::setUseInverterOnly(bool param)
{
    useInverterOnly = param;
}

bool Context::getTrackInverter()
{
    return trackInverter;
}

void Context::setTrackInverter(bool param)
{
    trackInverter = param;
}

bool Context::getTestOs()
{
    return testOs;
}

void Context::setTestOs(bool param)
{
    testOs = param;
}

bool Context::getUseInverter()
{
    return useInverter;
}

void Context::setUseInverter(bool param)
{
    useInverter = param;
}

void Context::readSettings()
{
    QSettings settings("hileben", "pvccs");

    osConfigInfo.setPlantCode(settings.value("plantCode", "None").toString());
    osConfigInfo.setFilterPressureHigh(settings.value("filterPressureHigh", "5").toString());
    osConfigInfo.setPump1PressureLow(settings.value("pump1PressureLow", "1").toString());
    osConfigInfo.setPump1PressureHigh(settings.value("pump1PressureHigh", "5").toString());
    osConfigInfo.setPump1CurrentShort(settings.value("pump1CurrentShort", "1").toString());
    osConfigInfo.setPump1CurrentOver(settings.value("pump1CurrentOver", "16").toString());
    osConfigInfo.setPump2PressureLow(settings.value("pump2PressureLow", "1").toString());
    osConfigInfo.setPump2PressureHigh(settings.value("pump2PressureHigh", "5").toString());
    osConfigInfo.setPump2CurrentShort(settings.value("pump2CurrentShort", "1").toString());
    osConfigInfo.setPump2CurrentOver(settings.value("pump2CurrentOver", "16").toString());
    osConfigInfo.setSprayProfileName(settings.value("sprayProfileName", "None").toString());
    osConfigInfo.setSprayProfileItems(settings.value("sprayProfileItems", "").toString());
    osConfigInfo.setSprayManualTime(settings.value("sprayManualTime", "1").toString());
    osConfigInfo.setPumpNumber(settings.value("pumpNumber", "1|1").toString());
    osConfigInfo.setSectorNumber(settings.value("sectorNumber", "1|1|1|1|1").toString());
    osConfigInfo.setFreezeTemperature(settings.value("freezeTemperature", "3").toString());
    osConfigInfo.setCloudyTemperature(settings.value("cloudyTemperature", "10").toString());
    osConfigInfo.setInverterMasterType(settings.value("inverterMasterType", "WebBox").toString());
    osConfigInfo.setInverterProtocolType(settings.value("inverterProtocolType", "PV-C3252S").toString());
    osConfigInfo.setInverterConnectionType(settings.value("inverterConnectionType", "Serial").toString());
    osConfigInfo.setInverterDeviceName(settings.value("inverterDeviceName", "/dev/ttyO4").toString());
    osConfigInfo.setInverterBaudrate(settings.value("inverterBaudrate", "9600").toString());
    osConfigInfo.setInverterIpAddress(settings.value("inverterIpAddress", "192.168.0.2").toString());
    osConfigInfo.setInverterPortNumber(settings.value("inverterPortNumber", "502").toString());
    osConfigInfo.setInverterSlaves(settings.value("inverterSlaves", "0+0+0|0+0+0|0+0+0|0+0+0").toString());
    osConfigInfo.setTemperatureCalibration(settings.value("temperatureCalibration", "0|0|0").toString());
    osConfigInfo.setSolarPowerCalibration(settings.value("solarPowerCalibration", "0").toString());
#if (BOPTS_FIX_VERSION_INFO == 0)
    osConfigInfo.setVersion(settings.value("version", "0.0.0").toString());
#else
    osConfigInfo.setVersion(settings.value("version", "0.0.0.0.0").toString());
#endif
}

void Context::writeSettings()
{
    QSettings settings("hileben", "pvccs");

    settings.setValue("plantCode", osConfigInfo.getPlantCode());
    settings.setValue("filterPressureHigh", osConfigInfo.getFilterPressureHigh());
    settings.setValue("pump1PressureLow", osConfigInfo.getPump1PressureLow());
    settings.setValue("pump1PressureHigh", osConfigInfo.getPump1PressureHigh());
    settings.setValue("pump1CurrentShort", osConfigInfo.getPump1CurrentShort());
    settings.setValue("pump1CurrentOver", osConfigInfo.getPump1CurrentOver());
    settings.setValue("pump2PressureLow", osConfigInfo.getPump2PressureLow());
    settings.setValue("pump2PressureHigh", osConfigInfo.getPump2PressureHigh());
    settings.setValue("pump2CurrentShort", osConfigInfo.getPump2CurrentShort());
    settings.setValue("pump2CurrentOver", osConfigInfo.getPump2CurrentOver());
    settings.setValue("sprayProfileName", osConfigInfo.getSprayProfileName());
    settings.setValue("sprayProfileItems", osConfigInfo.getSprayProfileItems());
    settings.setValue("sprayManualTime", osConfigInfo.getSprayManualTime());
    settings.setValue("pumpNumber", osConfigInfo.getPumpNumber());
    settings.setValue("sectorNumber", osConfigInfo.getSectorNumber());
    settings.setValue("freezeTemperature", osConfigInfo.getFreezeTemperature());
    settings.setValue("cloudyTemperature", osConfigInfo.getCloudyTemperature());
    settings.setValue("inverterMasterType", osConfigInfo.getInverterMasterType());
    settings.setValue("inverterProtocolType", osConfigInfo.getInverterProtocolType());
    settings.setValue("inverterConnectionType", osConfigInfo.getInverterConnectionType());
    settings.setValue("inverterDeviceName", osConfigInfo.getInverterDeviceName());
    settings.setValue("inverterBaudrate", osConfigInfo.getInverterBaudrate());
    settings.setValue("inverterIpAddress", osConfigInfo.getInverterIpAddress());
    settings.setValue("inverterPortNumber", osConfigInfo.getInverterPortNumber());
    settings.setValue("inverterSlaves", osConfigInfo.getInverterSlaves());
    settings.setValue("temperatureCalibration", osConfigInfo.getTemperatureCalibration());
    settings.setValue("solarPowerCalibration", osConfigInfo.getSolarPowerCalibration());
    settings.setValue("version", osConfigInfo.getVersion());
}
