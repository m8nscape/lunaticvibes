#include "setupdialog.h"
#include <QApplication>
#include "config/config_mgr.h"

#ifdef WIN32
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    int argc = 1;
    QApplication app(argc, &cmdline);
#else
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
#endif

    ConfigMgr::init();

    SetupDialog dialog;
    dialog.exec();

    return 0;
}
