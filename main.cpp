#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
    qDebug() << "OpenCV Version:" << CV_VERSION;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
