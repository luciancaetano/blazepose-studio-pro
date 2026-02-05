#ifndef PLUGINMANAGERWINDOW_H
#define PLUGINMANAGERWINDOW_H

#include <QDialog>

namespace Ui {
class PluginManagerWindow;
}

class PluginManagerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PluginManagerWindow(QWidget *parent = nullptr);
    ~PluginManagerWindow();

private:
    Ui::PluginManagerWindow *ui;
};

#endif // PLUGINMANAGERWINDOW_H
