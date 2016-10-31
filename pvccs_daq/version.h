#ifndef VERSION_H
#define VERSION_H

#include "buildopts.h"
#include <QString>

class Version
{
    static const uint MAJOR = 3;
    static const uint MINOR = 2;
    static const uint PATCH = 5;
    // 3.2.3 : Fix IO Board code.
    // 3.2.2 : Fix PVCCS code.

#if (BOPTS_FIX_VERSION_INFO == 1)
    static QString IO_VERSION;
#endif

    Version();


public:
    static QString getVersion();

#if (BOPTS_FIX_VERSION_INFO == 1)
    static QString getPVCCSVersion();
    static QString getIOVersion();
    static void setIOVersion(QString ioVer);
#endif

    /*
     * version : 1.0.0
     *    date : 2014-08-17
     * updater : kwlee
     * comment : begin
     */
};

#endif // VERSION_H
