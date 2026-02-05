#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class VideoGLWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_play_btn_clicked();

    void on_pause_btn_clicked();

    void on_add_01_clicked();

    void on_add_1_clicked();

    void on_minus_01_clicked();

    void on_minus_1_clicked();

private:
    void setupPlugins();

    Ui::MainWindow *ui;
    VideoGLWidget *videoWidget;
};
#endif // MAINWINDOW_H
