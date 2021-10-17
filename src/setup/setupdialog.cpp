#include "setupdialog.h"
#include "ui_setupdialog.h"

#include "common/meta.h"
#include "config/config_mgr.h"

SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);
}

SetupDialog::~SetupDialog()
{
    delete ui;
}
