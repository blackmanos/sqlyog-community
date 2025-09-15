// Entry point moved from the old WinMain implementation to a Qt based
// application.  The application now relies on QApplication and a Qt
// MainWindow rather than the Windows specific WinMain setup.

#include <QApplication>

#include "MainWindow.h"
#include "AppContext.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Ensure that the global application state is created.  Previously a
    // number of globals (for instance `pGlobals` and `g_accel`) lived in the
    // WinMain translation unit.  They are now encapsulated inside the
    // AppContext singleton which can be accessed anywhere in the program via
    // `AppContext::instance()`.
    AppContext::instance();

    MainWindow w;
    w.show();

    return app.exec();
}

