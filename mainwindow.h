#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>

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

private:
    void loadAndDisplayImage();
    
    Ui::MainWindow *ui;
    VideoGLWidget *videoWidget;
    cv::Mat currentImage;
};
#endif // MAINWINDOW_H
