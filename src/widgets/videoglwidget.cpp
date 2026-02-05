#include "widgets/videoglwidget.h"
#include <QDebug>
#include <QSurfaceFormat>
#include <QUrl>

VideoGLWidget::VideoGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_textureId(0)
    , m_hasFrame(false)
    , m_glInitialized(false)
    , m_frameTimer(nullptr)
    , m_mediaPlayer(nullptr)
    , m_audioOutput(nullptr)
    , m_fps(30.0)
    , m_totalFrames(0)
    , m_currentFrameIndex(0)
    , m_isPlaying(false)
    , m_syncCounter(0)
    , m_pluginManager(nullptr)
{
    qDebug() << "[VideoGLWidget] Constructor called";
    setMinimumSize(320, 240);

    // Configurar formato OpenGL para usar perfil de compatibilidade
    QSurfaceFormat format;
    format.setVersion(2, 1);  // OpenGL 2.1
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    setFormat(format);

    // Initialize timer for frame updates
    m_frameTimer = new QTimer(this);
    connect(m_frameTimer, &QTimer::timeout, this, &VideoGLWidget::updateVideoFrame);

    // Initialize media player for audio
    m_audioOutput = new QAudioOutput(this);
    m_mediaPlayer = new QMediaPlayer(this);
    m_mediaPlayer->setAudioOutput(m_audioOutput);

    // Inicializar gerenciador de plugins
    m_pluginManager = new VideoPluginManager(this);

    qDebug() << "[VideoGLWidget] OpenGL format configured: OpenGL 2.1 Compatibility";
}

VideoGLWidget::~VideoGLWidget()
{
    qDebug() << "[VideoGLWidget] Destructor called";
    stop();
    makeCurrent();
    deleteTexture();
    doneCurrent();
}

void VideoGLWidget::initializeGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    m_glInitialized = true;

    if (m_hasFrame && !m_currentFrame.empty()) {
        createTexture();
    }
}

void VideoGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void VideoGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (!m_hasFrame || m_textureId == 0) {
        return;
    }

    glLoadIdentity();

    // Calculate ratio to maintain image aspect ratio
    float imgAspect = static_cast<float>(m_currentFrame.cols) / m_currentFrame.rows;
    float widgetAspect = static_cast<float>(width()) / height();

    float scaleX = 1.0f;
    float scaleY = 1.0f;

    if (widgetAspect > imgAspect) {
        scaleX = imgAspect / widgetAspect;
    } else {
        scaleY = widgetAspect / imgAspect;
    }

    glBindTexture(GL_TEXTURE_2D, m_textureId);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-scaleX, -scaleY);
    glTexCoord2f(1.0f, 1.0f); glVertex2f( scaleX, -scaleY);
    glTexCoord2f(1.0f, 0.0f); glVertex2f( scaleX,  scaleY);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-scaleX,  scaleY);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void VideoGLWidget::updateFrame(const cv::Mat& frame)
{

    if (frame.empty()) {
        return;
    }

    // Convert to RGB if necessary (optimized - avoid unnecessary clone)
    cv::Mat rgbFrame;
    if (frame.channels() == 3) {
        cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);
    } else if (frame.channels() == 1) {
        cv::cvtColor(frame, rgbFrame, cv::COLOR_GRAY2RGB);
    } else if (frame.channels() == 4) {
        cv::cvtColor(frame, rgbFrame, cv::COLOR_BGRA2RGB);
    } else {
        rgbFrame = frame.clone();
    }

    // Processar frame com plugins (apenas se houver plugins habilitados)
    if (m_pluginManager && m_pluginManager->getEnabledPluginCount() > 0) {
        qint64 timestamp = position();
        m_pluginManager->processFrame(rgbFrame, timestamp, m_currentFrameIndex);
    }

    m_currentFrame = rgbFrame;
    m_hasFrame = true;


    // Only create texture if OpenGL is already initialized
    if (m_glInitialized) {
        makeCurrent();
        createTexture();
        doneCurrent();
    } else {
        qDebug() << "[VideoGLWidget] OpenGL not yet initialized, texture will be created in initializeGL";
    }
    update();
}

void VideoGLWidget::clearFrame()
{
    makeCurrent();
    deleteTexture();
    doneCurrent();

    m_currentFrame.release();
    m_hasFrame = false;
    update();
}

void VideoGLWidget::createTexture()
{

    if (m_currentFrame.empty()) {
        qWarning() << "[VideoGLWidget] currentFrame empty, cannot create texture";
        return;
    }

    deleteTexture();

    // Gerar textura OpenGL
    glGenTextures(1, &m_textureId);

    glBindTexture(GL_TEXTURE_2D, m_textureId);

    // Configure texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Carregar dados da imagem na textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 m_currentFrame.cols, m_currentFrame.rows,
                 0, GL_RGB, GL_UNSIGNED_BYTE,
                 m_currentFrame.data);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        qWarning() << "[VideoGLWidget] OpenGL error after glTexImage2D:" << error;
    } else {
        qDebug() << "[VideoGLWidget] Texture created successfully!";
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void VideoGLWidget::deleteTexture()
{
    if (m_textureId != 0) {
        glDeleteTextures(1, &m_textureId);
        m_textureId = 0;
    }
}

// ==================== VIDEO CONTROLS ====================

bool VideoGLWidget::loadVideo(const QString& videoPath)
{
    qDebug() << "[VideoGLWidget] Loading video:" << videoPath;

    // Stop previous video if any
    stop();

    m_videoPath = videoPath;

    // Abrir vídeo com OpenCV
    m_videoCapture.open(videoPath.toStdString());

    if (!m_videoCapture.isOpened()) {
        qWarning() << "[VideoGLWidget] Failed to open video with OpenCV";
        return false;
    }

    // Get video properties
    m_fps = m_videoCapture.get(cv::CAP_PROP_FPS);
    if (m_fps <= 0) m_fps = 30.0;

    m_totalFrames = static_cast<qint64>(m_videoCapture.get(cv::CAP_PROP_FRAME_COUNT));
    int width = static_cast<int>(m_videoCapture.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(m_videoCapture.get(cv::CAP_PROP_FRAME_HEIGHT));

    qDebug() << "[VideoGLWidget] Video loaded:";
    qDebug() << "  Dimensions:" << width << "x" << height;
    qDebug() << "  FPS:" << m_fps;
    qDebug() << "  Total frames:" << m_totalFrames;
    qDebug() << "  Duration:" << (m_totalFrames / m_fps) << "seconds";

    // Configure audio
    m_mediaPlayer->setSource(QUrl::fromLocalFile(videoPath));
    m_audioOutput->setVolume(1.0);

    m_currentFrameIndex = 0;

    // Initialize plugins with video information
    if (m_pluginManager) {
        QVariantMap videoInfo;
        videoInfo["width"] = width;
        videoInfo["height"] = height;
        videoInfo["fps"] = m_fps;
        videoInfo["totalFrames"] = m_totalFrames;
        videoInfo["duration"] = static_cast<qint64>((m_totalFrames / m_fps) * 1000.0);
        m_pluginManager->initializePlugins(videoInfo);
    }

    return true;
}

void VideoGLWidget::play()
{
    if (!m_videoCapture.isOpened()) {
        qWarning() << "[VideoGLWidget] No video loaded";
        return;
    }

    if (m_isPlaying) {
        qDebug() << "[VideoGLWidget] Video is already playing";
        return;
    }

    qDebug() << "[VideoGLWidget] Starting playback";
    m_isPlaying = true;

    // Notificar plugins
    if (m_pluginManager) {
        m_pluginManager->notifyPlaybackStarted();
    }

    // Start audio FIRST (it is the master clock)
    m_mediaPlayer->play();

    // Fast timer to update video based on audio position
    // Verifica a cada 16ms (~60fps) qual frame deveria estar exibindo
    m_frameTimer->start(16);
}

void VideoGLWidget::pause()
{
    if (!m_isPlaying) {
        qDebug() << "[VideoGLWidget] Video is already paused";
        return;
    }

    qDebug() << "[VideoGLWidget] Pausing playback";
    m_isPlaying = false;

    // Notificar plugins
    if (m_pluginManager) {
        m_pluginManager->notifyPlaybackPaused();
    }

    // Parar timer
    m_frameTimer->stop();

    // Pause audio
    m_mediaPlayer->pause();
}

void VideoGLWidget::stop()
{
    qDebug() << "[VideoGLWidget] Stopping playback";

    m_isPlaying = false;

    // Notificar plugins
    if (m_pluginManager) {
        m_pluginManager->notifyPlaybackStopped();
    }

    if (m_frameTimer) {
        m_frameTimer->stop();
    }

    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
    }

    if (m_videoCapture.isOpened()) {
        m_videoCapture.release();
    }

    m_currentFrameIndex = 0;
    clearFrame();
}

void VideoGLWidget::seek(qint64 positionMs)
{
    if (!m_videoCapture.isOpened()) {
        qWarning() << "[VideoGLWidget] No video loaded";
        return;
    }

    qDebug() << "[VideoGLWidget] Seeking position:" << positionMs << "ms";

    // Calculate corresponding frame
    double positionSec = positionMs / 1000.0;
    qint64 targetFrame = static_cast<qint64>(positionSec * m_fps);

    // Limitar aos bounds
    if (targetFrame < 0) targetFrame = 0;
    if (targetFrame >= m_totalFrames) targetFrame = m_totalFrames - 1;

    // Position video
    m_videoCapture.set(cv::CAP_PROP_POS_FRAMES, targetFrame);
    m_currentFrameIndex = targetFrame;

    // Position audio
    m_mediaPlayer->setPosition(positionMs);
    
    // Notificar plugins
    if (m_pluginManager) {
        m_pluginManager->notifySeek(positionMs);
    }
    
    // Ler e exibir o frame atual
    cv::Mat frame;
    if (m_videoCapture.read(frame) && !frame.empty()) {
        updateFrame(frame);
        m_currentFrameIndex++;
    }

    qDebug() << "[VideoGLWidget] Positioned at frame:" << targetFrame;
}

void VideoGLWidget::forward(qint64 ms)
{
    qint64 currentPos = position();
    qint64 newPos = currentPos + ms;
    qDebug() << "[VideoGLWidget] Forwarding" << ms << "ms";
    
    bool wasPlaying = m_isPlaying;
    if (wasPlaying) {
        pause();
    }
    
    seek(newPos);
}

void VideoGLWidget::backward(qint64 ms)
{
    qint64 currentPos = position();
    qint64 newPos = currentPos - ms;
    qDebug() << "[VideoGLWidget] Rewinding" << ms << "ms";
    
    bool wasPlaying = m_isPlaying;
    if (wasPlaying) {
        pause();
    }
    
    seek(newPos);
}

bool VideoGLWidget::isPlaying() const
{
    return m_isPlaying;
}

qint64 VideoGLWidget::position() const
{
    if (!m_videoCapture.isOpened()) {
        return 0;
    }
    return static_cast<qint64>((m_currentFrameIndex / m_fps) * 1000.0);
}

qint64 VideoGLWidget::duration() const
{
    if (!m_videoCapture.isOpened()) {
        return 0;
    }
    return static_cast<qint64>((m_totalFrames / m_fps) * 1000.0);
}

double VideoGLWidget::getFps() const
{
    return m_fps;
}

VideoPluginManager* VideoGLWidget::pluginManager()
{
    return m_pluginManager;
}

void VideoGLWidget::updateVideoFrame()
{
    if (!m_videoCapture.isOpened() || !m_isPlaying) {
        return;
    }

    // Get current audio position (master clock)
    qint64 audioPos = m_mediaPlayer->position();

    // Calcular qual frame deveria estar sendo exibido
    qint64 targetFrame = static_cast<qint64>((audioPos / 1000.0) * m_fps);

    // If already at correct frame or ahead, do nothing
    if (m_currentFrameIndex >= targetFrame) {
        return;
    }

    // Se estamos muito atrasados (>5 frames), pular frames
    if (targetFrame - m_currentFrameIndex > 5) {
        qDebug() << "[VideoGLWidget] Skipping frames to catch up with audio. From" << m_currentFrameIndex << "to" << targetFrame;
        m_videoCapture.set(cv::CAP_PROP_POS_FRAMES, targetFrame);
        m_currentFrameIndex = targetFrame;
    }

    // Read and display frames until reaching audio position
    while (m_currentFrameIndex < targetFrame) {
        cv::Mat frame;
        bool success = m_videoCapture.read(frame);

        if (!success || frame.empty()) {
            qDebug() << "[VideoGLWidget] End of video reached";
            stop();
            return;
        }

        m_currentFrameIndex++;

        // Only update screen on last necessary frame
        if (m_currentFrameIndex >= targetFrame) {
            updateFrame(frame);
        }
    }
}

void VideoGLWidget::syncAudioToVideo()
{
    // Method available for manual synchronization if necessary
    // Not called automatically to avoid audio stuttering
    qint64 videoPos = position();
    qint64 audioPos = m_mediaPlayer->position();
    qint64 diff = videoPos - audioPos;

    if (qAbs(diff) > 500) {
        qDebug() << "[VideoGLWidget] Manual sync - Diff:" << diff << "ms";
        m_mediaPlayer->setPosition(videoPos);
    }
}
