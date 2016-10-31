#include "invbuilder.h"
#include "pvc3252sserprotocol.h"

//static
QString InvBuilder::connectionType = "";

InvBuilder::InvBuilder()
{
}

//static
InvInterface* InvBuilder::makeInvInterface(InvConfigInfo invConfigInfo)
{
    InvInterface* invInterface = 0;

    if (invConfigInfo.getMasterType() == "OS") {
        if (invConfigInfo.getConnectionType() == "Serial") {
            if (invConfigInfo.getProtocolType() == "PV-C3252S") {
                invInterface = new Pvc3252sSerProtocol;
            }

            if (invInterface != 0) {
                connectionType = invConfigInfo.getConnectionType();

                ((InvSerProtocol*) invInterface)->setTimeout(1);
                ((InvSerProtocol*) invInterface)->openProtocol(invConfigInfo.getDeviceName(), invConfigInfo.getBaudrate().toLong());
            }
        }
        else { // LAN

        }
    }

    return invInterface;
}

//static
void InvBuilder::destoryInvInterface(InvInterface* invInterface)
{
    if (invInterface) {
        if (invInterface->isOpen()) {
            invInterface->closeProtocol();
        }

        if (connectionType == "Serial") {
            ((InvSerProtocol*) invInterface)->deleteLater();
        }
        else { // LAN

        }

        invInterface = 0;
    }
}
