#ifndef OVERLAYVIDEOPLUGIN_H
#define OVERLAYVIDEOPLUGIN_H

#include "ivideoplugin.h"
#include <QColor>
#include <QFont>

/**
 * @brief Example plugin that draws overlays on video
 * 
 * This plugin demonstrates how to implement IVideoPlugin to draw
 * information about the video (FPS, timestamp, frame count, etc)
 */
class OverlayVideoPlugin : public IVideoPlugin
{
public:
    OverlayVideoPlugin();
    virtual ~OverlayVideoPlugin() = default;

    // IVideoPlugin interface
    bool processFrame(cv::Mat& frame, qint64 timestamp, qint64 frameIndex) override;
    void initialize(const QVariantMap& videoInfo) override;
    void finalize() override;
    
    void onPlaybackStarted() override;
    void onPlaybackPaused() override;
    void onPlaybackStopped() override;
    void onSeek(qint64 position) override;

    QString getName() const override;
    QString getVersion() const override;
    QString getDescription() const override;
    
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    void setSettings(const QVariantMap& settings) override;
    QVariantMap getSettings() const override;
    
    int getPriority() const override;

    // Overlay specific settings
    void setShowFPS(bool show);
    void setShowTimestamp(bool show);
    void setShowFrameCount(bool show);
    void setShowResolution(bool show);
    void setTextColor(const QColor& color);
    void setBackgroundOpacity(double opacity);

private:
    void drawOverlay(cv::Mat& frame, qint64 timestamp, qint64 frameIndex);
    void drawText(cv::Mat& frame, const QString& text, int x, int y);
    QString formatTimestamp(qint64 timestampMs);

    bool m_enabled;
    
    // Video information
    int m_videoWidth;
    int m_videoHeight;
    double m_videoFps;
    qint64 m_videoDuration;
    
    // Display settings
    bool m_showFPS;
    bool m_showTimestamp;
    bool m_showFrameCount;
    bool m_showResolution;
    QColor m_textColor;
    double m_backgroundOpacity;
    
    // Playback state
    bool m_isPlaying;
    
    // Real FPS calculation
    qint64 m_lastFrameTime;
    double m_currentFPS;
    int m_frameCounter;
};

#endif // OVERLAYVIDEOPLUGIN_H
