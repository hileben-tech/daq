#include "version.h"
#include "statusinfo.h"
#include <QDebug>

#if (BOPTS_FIX_VERSION_INFO == 1)
QString Version::IO_VERSION = "0.0";
#endif

Version::Version()
{
}

//static
QString Version::getVersion()
{
#if (BOPTS_FIX_VERSION_INFO == 1)
    QString pvccsVer = QString("%1.%2.%3-").arg(MAJOR).arg(MINOR).arg(PATCH);
    return pvccsVer.append(IO_VERSION);
#else
    return QString("%1.%2.%3").arg(MAJOR).arg(MINOR).arg(PATCH);
#endif
}

#if (BOPTS_FIX_VERSION_INFO == 1)
QString Version::getPVCCSVersion()
{
    return QString("%1.%2.%3").arg(MAJOR).arg(MINOR).arg(PATCH);
}

QString Version::getIOVersion()
{
    return IO_VERSION;
}

void Version::setIOVersion(QString ioVer)
{
    IO_VERSION = ioVer;
    //qDebug() << "[Version::setIOVersion]" << IO_VERSION;
}
#endif
