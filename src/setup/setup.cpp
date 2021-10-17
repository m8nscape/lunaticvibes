#include "setupdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SetupDialog dialog;
    int ret = dialog.exec();
    return ret;
}
