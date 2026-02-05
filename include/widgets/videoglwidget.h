#ifndef VIDEOGLWIDGET_H
#define VIDEOGLWIDGET_H

#include <QOpenGLWidget>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <opencv2/opencv.hpp>
#include "core/videopluginmanager.h"

class VideoGLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit VideoGLWidget(QWidget *parent = nullptr);
    ~VideoGLWidget();

    // Method to update displayed image/frame
    void updateFrame(const cv::Mat& frame);

    // Method to clear screen
    void clearFrame();

    // Video controls
    bool loadVideo(const QString& videoPath);
    void play();
    void pause();
    void stop();
    void seek(qint64 positionMs);
    void forward(qint64 ms);
    void backward(qint64 ms);

    // Getters
    bool isPlaying() const;
    qint64 position() const;
    qint64 duration() const;
    double getFps() const;

    // Plugin manager
    VideoPluginManager* pluginManager();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private slots:
    void updateVideoFrame();

private:
    void createTexture();
    void deleteTexture();
    void syncAudioToVideo();

    // OpenGL rendering
    GLuint m_textureId;
    cv::Mat m_currentFrame;
    bool m_hasFrame;
    bool m_glInitialized;

    // Video playback
    cv::VideoCapture m_videoCapture;
    QTimer* m_frameTimer;
    QMediaPlayer* m_mediaPlayer;
    QAudioOutput* m_audioOutput;

    QString m_videoPath;
    double m_fps;
    qint64 m_totalFrames;
    qint64 m_currentFrameIndex;
    bool m_isPlaying;
    int m_syncCounter;

    // Plugin system
    VideoPluginManager* m_pluginManager;
};

#endif // VIDEOGLWIDGET_H
