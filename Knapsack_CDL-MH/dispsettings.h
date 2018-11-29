#ifndef DISPSETTINGS_H
#define DISPSETTINGS_H
#include <global_defines.h>
#include <QDebug>

class DispSettings
{
public:
    DispSettings(SOFTWARESETTINGS settings);
private:
    SOFTWARESETTINGS dispSettings;
};

#endif // DISPSETTINGS_H
