#include "setupdialog.h"
#include <QApplication>
#include "config/config_mgr.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ConfigMgr::init();

    SetupDialog dialog;
    dialog.exec();

    return 0;
}
