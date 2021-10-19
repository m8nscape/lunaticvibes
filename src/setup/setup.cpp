#include "setupdialog.h"
#include <QApplication>
#include "config/config_mgr.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ConfigMgr::init();
    ConfigMgr::load();
    int ret = 0;
    {
        SetupDialog dialog;
        ret = dialog.exec();
    }
    return ret;
}
