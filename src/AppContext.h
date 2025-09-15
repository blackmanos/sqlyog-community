#pragma once

#include <QObject>

#include "Global.h"

// Simple singleton used to encapsulate application wide state that previously
// lived in global variables.  Only a minimal subset is kept here to demonstrate
// the transition away from globals.
class AppContext : public QObject
{
    Q_OBJECT

public:
    static AppContext &instance();

    GLOBALS globals;        // replaces the old `pGlobals` global pointer

private:
    AppContext();
};

