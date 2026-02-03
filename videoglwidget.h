#ifndef VIDEOGLWIDGET_H
#define VIDEOGLWIDGET_H

#include <QOpenGLWidget>
#include <opencv2/opencv.hpp>

class VideoGLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit VideoGLWidget(QWidget *parent = nullptr);
    ~VideoGLWidget();

    // Método para atualizar a imagem/frame exibido
    void updateFrame(const cv::Mat& frame);
    
    // Método para limpar a tela
    void clearFrame();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void createTexture();
    void deleteTexture();

    GLuint m_textureId;
    cv::Mat m_currentFrame;
    bool m_hasFrame;
    bool m_glInitialized;
};

#endif // VIDEOGLWIDGET_H
