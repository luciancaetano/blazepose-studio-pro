#include "plugins/overlayvideoplugin.h"
#include <QDebug>
#include <QDateTime>
#include <opencv2/imgproc.hpp>

OverlayVideoPlugin::OverlayVideoPlugin()
    : m_enabled(true)
    , m_videoWidth(0)
    , m_videoHeight(0)
    , m_videoFps(0.0)
    , m_videoDuration(0)
    , m_showFPS(true)
    , m_showTimestamp(false)  // Disabled by default for performance
    , m_showFrameCount(false)  // Disabled by default for performance
    , m_showResolution(false)
    , m_textColor(Qt::white)
    , m_backgroundOpacity(0.5)
    , m_isPlaying(false)
    , m_lastFrameTime(0)
    , m_currentFPS(0.0)
    , m_frameCounter(0)
{
}

bool OverlayVideoPlugin::processFrame(cv::Mat& frame, qint64 timestamp, qint64 frameIndex)
{
    if (!m_enabled || frame.empty()) {
        return false;
    }

    m_frameCounter++;
    
    // Calculate real FPS only every 5 frames (optimization)
    if (m_frameCounter % 5 == 0) {
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        if (m_lastFrameTime > 0) {
            qint64 delta = currentTime - m_lastFrameTime;
            if (delta > 0) {
                double instantFPS = 5000.0 / delta;  // 5 frames / delta
                m_currentFPS = m_currentFPS * 0.8 + instantFPS * 0.2;
            }
        }
        m_lastFrameTime = currentTime;
    }

    // Desenhar overlay
    drawOverlay(frame, timestamp, frameIndex);

    return true;
}

void OverlayVideoPlugin::initialize(const QVariantMap& videoInfo)
{
    m_videoWidth = videoInfo.value("width", 0).toInt();
    m_videoHeight = videoInfo.value("height", 0).toInt();
    m_videoFps = videoInfo.value("fps", 0.0).toDouble();
    m_videoDuration = videoInfo.value("duration", 0).toLongLong();
    
    m_frameCounter = 0;
    m_lastFrameTime = 0;
    m_currentFPS = 0.0;

    qDebug() << "[OverlayVideoPlugin] Initialized with video:"
             << m_videoWidth << "x" << m_videoHeight
             << "@" << m_videoFps << "fps";
}

void OverlayVideoPlugin::finalize()
{
    qDebug() << "[OverlayVideoPlugin] Finalized. Total frames processed:" << m_frameCounter;
    m_frameCounter = 0;
}

void OverlayVideoPlugin::onPlaybackStarted()
{
    m_isPlaying = true;
    m_lastFrameTime = 0;
    qDebug() << "[OverlayVideoPlugin] Playback started";
}

void OverlayVideoPlugin::onPlaybackPaused()
{
    m_isPlaying = false;
    qDebug() << "[OverlayVideoPlugin] Playback paused";
}

void OverlayVideoPlugin::onPlaybackStopped()
{
    m_isPlaying = false;
    m_frameCounter = 0;
    qDebug() << "[OverlayVideoPlugin] Playback stopped";
}

void OverlayVideoPlugin::onSeek(qint64 position)
{
    m_lastFrameTime = 0; // Reset FPS calculation
    qDebug() << "[OverlayVideoPlugin] Seek to position:" << position << "ms";
}

QString OverlayVideoPlugin::getName() const
{
    return "Overlay Plugin";
}

QString OverlayVideoPlugin::getVersion() const
{
    return "1.0.0";
}

QString OverlayVideoPlugin::getDescription() const
{
    return "Draws information about the video (FPS, timestamp, etc)";
}

bool OverlayVideoPlugin::isEnabled() const
{
    return m_enabled;
}

void OverlayVideoPlugin::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

void OverlayVideoPlugin::setSettings(const QVariantMap& settings)
{
    m_showFPS = settings.value("showFPS", true).toBool();
    m_showTimestamp = settings.value("showTimestamp", true).toBool();
    m_showFrameCount = settings.value("showFrameCount", true).toBool();
    m_showResolution = settings.value("showResolution", false).toBool();
    m_backgroundOpacity = settings.value("backgroundOpacity", 0.5).toDouble();
    
    if (settings.contains("textColor")) {
        m_textColor = settings.value("textColor").value<QColor>();
    }
}

QVariantMap OverlayVideoPlugin::getSettings() const
{
    QVariantMap settings;
    settings["showFPS"] = m_showFPS;
    settings["showTimestamp"] = m_showTimestamp;
    settings["showFrameCount"] = m_showFrameCount;
    settings["showResolution"] = m_showResolution;
    settings["backgroundOpacity"] = m_backgroundOpacity;
    settings["textColor"] = m_textColor;
    return settings;
}

int OverlayVideoPlugin::getPriority() const
{
    return 1000; // Execute last (draw over everything)
}

void OverlayVideoPlugin::setShowFPS(bool show)
{
    m_showFPS = show;
}

void OverlayVideoPlugin::setShowTimestamp(bool show)
{
    m_showTimestamp = show;
}

void OverlayVideoPlugin::setShowFrameCount(bool show)
{
    m_showFrameCount = show;
}

void OverlayVideoPlugin::setShowResolution(bool show)
{
    m_showResolution = show;
}

void OverlayVideoPlugin::setTextColor(const QColor& color)
{
    m_textColor = color;
}

void OverlayVideoPlugin::setBackgroundOpacity(double opacity)
{
    m_backgroundOpacity = qBound(0.0, opacity, 1.0);
}

void OverlayVideoPlugin::drawOverlay(cv::Mat& frame, qint64 timestamp, qint64 frameIndex)
{
    int yOffset = 30;
    int lineHeight = 30;
    int xPos = 10;

    // Converter cor Qt para OpenCV
    cv::Scalar cvColor(m_textColor.blue(), m_textColor.green(), m_textColor.red());

    if (m_showFPS) {
        QString fpsText = QString("FPS: %1 / %2").arg(m_currentFPS, 0, 'f', 1).arg(m_videoFps, 0, 'f', 1);
        drawText(frame, fpsText, xPos, yOffset);
        yOffset += lineHeight;
    }

    if (m_showTimestamp) {
        QString timeText = QString("Time: %1").arg(formatTimestamp(timestamp));
        drawText(frame, timeText, xPos, yOffset);
        yOffset += lineHeight;
    }

    if (m_showFrameCount) {
        QString frameText = QString("Frame: %1").arg(frameIndex);
        drawText(frame, frameText, xPos, yOffset);
        yOffset += lineHeight;
    }

    if (m_showResolution) {
        QString resText = QString("Resolution: %1x%2").arg(frame.cols).arg(frame.rows);
        drawText(frame, resText, xPos, yOffset);
        yOffset += lineHeight;
    }

    // Playback status
    if (m_isPlaying) {
        drawText(frame, "PLAYING", frame.cols - 150, 30);
    } else {
        drawText(frame, "PAUSED", frame.cols - 150, 30);
    }
}

void OverlayVideoPlugin::drawText(cv::Mat& frame, const QString& text, int x, int y)
{
    std::string stdText = text.toStdString();
    
    // Converter cor Qt para OpenCV (BGR)
    cv::Scalar textColor(m_textColor.blue(), m_textColor.green(), m_textColor.red());
    cv::Scalar bgColor(0, 0, 0, m_backgroundOpacity * 255);
    
    // Font settings
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.6;
    int thickness = 1;
    int baseline = 0;
    
    // Calcular tamanho do texto
    cv::Size textSize = cv::getTextSize(stdText, fontFace, fontScale, thickness, &baseline);
    
    // Desenhar fundo semi-transparente
    int padding = 5;
    cv::Rect bgRect(x - padding, y - textSize.height - padding,
                    textSize.width + 2 * padding, textSize.height + 2 * padding);
    
    // Ensure rectangle is within frame bounds
    bgRect.x = std::max(0, bgRect.x);
    bgRect.y = std::max(0, bgRect.y);
    bgRect.width = std::min(bgRect.width, frame.cols - bgRect.x);
    bgRect.height = std::min(bgRect.height, frame.rows - bgRect.y);
    
    if (bgRect.width > 0 && bgRect.height > 0) {
        // Draw semi-transparent background directly (optimized - no clone)
        cv::Mat roi = frame(bgRect);
        cv::Mat bg(bgRect.height, bgRect.width, frame.type(), cv::Scalar(0, 0, 0));
        cv::addWeighted(roi, 1.0 - m_backgroundOpacity, bg, m_backgroundOpacity, 0, roi);
    }
    
    // Desenhar texto
    cv::putText(frame, stdText, cv::Point(x, y), fontFace, fontScale, textColor, thickness, cv::LINE_AA);
}

QString OverlayVideoPlugin::formatTimestamp(qint64 timestampMs)
{
    int totalSeconds = timestampMs / 1000;
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    int milliseconds = timestampMs % 1000;
    
    return QString("%1:%2:%3.%4")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(milliseconds, 3, 10, QChar('0'));
}
