#include "ui/mainwindow.h"
#include "./ui_mainwindow.h"
#include "widgets/videoglwidget.h"
#include "plugins/overlayvideoplugin.h"
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <memory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , videoWidget(nullptr)
{
    ui->setupUi(this);

    // Get reference to promoted OpenGL widget
    videoWidget = ui->openGLWidget;

    // Setup plugin system
    setupPlugins();

    // // Try to load video automatically
    // QString videoPath = "video.mp4";
    // QFile videoFile(videoPath);

    // // Try alternative paths
    // if (!videoFile.exists()) {
    //     videoPath = "../video.mp4";
    //     videoFile.setFileName(videoPath);
    // }

    // if (!videoFile.exists()) {
    //     videoPath = "C:/workspace/blazestudioprovs/video.mp4";
    //     videoFile.setFileName(videoPath);
    // }

    // // If found video, load and play
    // if (videoFile.exists()) {
    //     qDebug() << "Video found, starting playback:" << videoPath;
    //     if (videoWidget->loadVideo(videoPath)) {
    //         videoWidget->play();
    //     }
    // } else {
    //     qDebug() << "Video not found. Place a video.mp4 file in the project";
    //     QMessageBox::information(this, "Info",
    //         "No video found. Place a 'video.mp4' file in the project folder.");
    // }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupPlugins()
{
    if (!videoWidget) {
        return;
    }

    // Get plugin manager
    VideoPluginManager* pluginManager = videoWidget->pluginManager();
    if (!pluginManager) {
        qWarning() << "Plugin manager not available";
        return;
    }

    // Create and add overlay plugin
    auto overlayPlugin = std::make_shared<OverlayVideoPlugin>();
    
    // Configure plugin (optimized - less info = better performance)
    overlayPlugin->setShowFPS(true);
    overlayPlugin->setShowTimestamp(false);  // Disabled for performance
    overlayPlugin->setShowFrameCount(false); // Disabled for performance
    overlayPlugin->setShowResolution(false); // Disabled for performance
    overlayPlugin->setTextColor(Qt::green);
    overlayPlugin->setBackgroundOpacity(0.5);
    
    // Add to manager
    pluginManager->addPlugin(overlayPlugin);
    
    qDebug() << "Plugins configured successfully!";
    qDebug() << "Total de plugins:" << pluginManager->getPluginCount();
}

void MainWindow::on_actionOpen_triggered()
{
    // Create filter for common video formats
    QString filter = "Video Files (*.mp4 *.avi *.mkv *.mov *.wmv *.flv *.webm *.m4v);;";
    filter += "MP4 (*.mp4);;";
    filter += "AVI (*.avi);;";
    filter += "MKV (*.mkv);;";
    filter += "All files (*.*)";
    
    // Open file selection dialog
    QString videoPath = QFileDialog::getOpenFileName(
        this,
        "Open Video",
        QDir::homePath(),  // Diretório inicial
        filter
    );
    
    // If user cancelled, return
    if (videoPath.isEmpty()) {
        qDebug() << "Video selection cancelled";
        return;
    }
    
    qDebug() << "Video selected:" << videoPath;
    
    // Load and play video
    if (videoWidget->loadVideo(videoPath)) {
        videoWidget->play();
        qDebug() << "Playback started!";
    } else {
        QMessageBox::warning(this, "Error", 
            QString("Could not open video:\n%1").arg(videoPath));
    }
}


void MainWindow::on_play_btn_clicked()
{
    if (videoWidget) {
        videoWidget->play();
        qDebug() << "Play clicked";
    }
}


void MainWindow::on_pause_btn_clicked()
{
    if (videoWidget) {
        videoWidget->pause();
        qDebug() << "Pause clicked";
    }
}


void MainWindow::on_add_01_clicked()
{
    if (videoWidget) {
        videoWidget->forward(100);  // 0.1 second = 100ms
        qDebug() << "Forwarded 0.1s";
    }
}


void MainWindow::on_add_1_clicked()
{
    if (videoWidget) {
        videoWidget->forward(1000);  // 1 second = 1000ms
        qDebug() << "Forwarded 1s";
    }
}


void MainWindow::on_minus_01_clicked()
{
    if (videoWidget) {
        videoWidget->backward(100);  // 0.1 second = 100ms
        qDebug() << "Rewound 0.1s";
    }
}


void MainWindow::on_minus_1_clicked()
{
    if (videoWidget) {
        videoWidget->backward(1000);  // 1 second = 1000ms
        qDebug() << "Rewound 1s";
    }
}

