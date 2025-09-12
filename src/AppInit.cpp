#include "AppInit.h"
#include "CommonHelper.h"
#include "GUIHelper.h"

PGLOBALS pGlobals = nullptr;

bool InitializeApp(int argc, char** argv)
{
    pGlobals = new GLOBALS;
    if (!pGlobals)
        return false;
    // initialize MySQL library
    if (mysql_library_init(0, nullptr, nullptr))
        return false;
    // setup global state
    InitGlobals(pGlobals);
    // initialize networking
    InitWinSock();
    return true;
}

void ShutdownApp()
{
    mysql_library_end();
#ifdef _WIN32
    WSACleanup();
#endif
    delete pGlobals;
    pGlobals = nullptr;
}
