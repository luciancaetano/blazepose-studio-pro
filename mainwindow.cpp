#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "videoglwidget.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , videoWidget(nullptr)
{
    ui->setupUi(this);
    
    // Obter referência ao widget OpenGL promovido
    videoWidget = ui->openGLWidget;
    
    // Carregar e exibir a imagem pessoa.jpg
    loadAndDisplayImage();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadAndDisplayImage()
{
    // Tentar carregar a imagem pessoa.jpg do diretório atual
    std::string imagePath = "pessoa.jpg";
    currentImage = cv::imread(imagePath);
    
    if (currentImage.empty()) {
        // Tentar caminho relativo ao executável (pasta build)
        imagePath = "../pessoa.jpg";
        currentImage = cv::imread(imagePath);
    }
    
    if (currentImage.empty()) {
        // Tentar na pasta models
        imagePath = "models/pessoa.jpg";
        currentImage = cv::imread(imagePath);
    }
    
    if (currentImage.empty()) {
        // Tentar caminho absoluto baseado no diretório do projeto
        imagePath = "c:/workspace/blazestudioprovs/pessoa.jpg";
        currentImage = cv::imread(imagePath);
    }
    
    if (currentImage.empty()) {
        QString msg = QString("Não foi possível carregar a imagem pessoa.jpg\n"
                             "Verifique se o arquivo existe no diretório do projeto.");
        QMessageBox::warning(this, "Erro ao Carregar Imagem", msg);
        qWarning() << "Falha ao carregar pessoa.jpg - verifique o caminho do arquivo";
        return;
    }
    
    qDebug() << "Imagem carregada com sucesso:" << imagePath.c_str();
    qDebug() << "Dimensões:" << currentImage.cols << "x" << currentImage.rows;
    qDebug() << "Canais:" << currentImage.channels();
    
    // Exibir a imagem no widget OpenGL
    if (videoWidget) {
        videoWidget->updateFrame(currentImage);
        qDebug() << "Frame enviado para VideoGLWidget";
    } else {
        qWarning() << "VideoGLWidget não foi inicializado corretamente";
    }
}
