#include "ui/pluginmanagerwindow.h"
#include "ui_pluginmanagerwindow.h"

PluginManagerWindow::PluginManagerWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PluginManagerWindow)
{
    ui->setupUi(this);
}

PluginManagerWindow::~PluginManagerWindow()
{
    delete ui;
}
