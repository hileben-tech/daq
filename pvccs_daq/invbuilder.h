#ifndef INVBUILDER_H
#define INVBUILDER_H

#include "invinterface.h"
#include "invconfiginfo.h"

class InvBuilder
{
    InvBuilder();

    static QString connectionType;

public:
    static InvInterface* makeInvInterface(InvConfigInfo invConfigInfo);
    static void destoryInvInterface(InvInterface* invInterface);

};

#endif // INVBUILDER_H
