#include "os.h"
#include "invconfiginfo.h"
#include "invinfo.h"
#include "ledinfo.h"
#include "statusinfo.h"
#include "context.h"
#include "version.h"
#include "utility.h"
#include "sckcmodbus.h"
#include <QCoreApplication>
#include <QStringList>

#include "modbusrtuprotocol.h"

int main(int argc, char *argv[])
{
#if (BOPTS_FIX_VERSION_INFO == 1)
    qDebug() << "==================================";
    qDebug() << " PVCCS Version : " << Version::getPVCCSVersion();
    qDebug() << "==================================";
#else
    qDebug() << "==================================";
    qDebug() << " PVCCS Version : " << Version::getVersion();
    qDebug() << "==================================";
#endif

    qRegisterMetaType<InvConfigInfo>("InvConfigInfo");
    qRegisterMetaType<InvInfo>("InvInfo");
    qRegisterMetaType<LedInfo>("LedInfo");
    qRegisterMetaType<StatusInfo>("StatusInfo");

    QCoreApplication a(argc, argv);

    Context* ctx = Context::getInstance();

    QStringList arguments = QCoreApplication::arguments();


    foreach (QString argument, arguments) {
        if (argument == "--UseInverterOnly") {
            ctx->setUseInverterOnly(true);
        }
        else if (argument == "--TrackInverter") {
            ctx->setTrackInverter(true);
        }
        else if (argument == "--test") {
            ctx->setTestOs(true);
        }
        else if (argument == "--DontUseInverter") {
            ctx->setUseInverter(false);
        }
    }

    OS os;

    if (!ctx->getUseInverterOnly()) {
        os.start();
    }

    return a.exec();
}
