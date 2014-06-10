#include "DirectoryWatcher.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("PowerOne");
    QCoreApplication::setOrganizationDomain("power.one");
    QCoreApplication::setApplicationName("PowerOne Directory Watcher");

    DirectoryWatcher* dw = new DirectoryWatcher();

    return app.exec();
}
