#ifndef IVIDEOPLUGIN_H
#define IVIDEOPLUGIN_H

#include <opencv2/opencv.hpp>
#include <QString>
#include <QVariantMap>

/**
 * @brief Abstract interface for video processing plugins
 * 
 * This class defines the interface that all video plugins must implement.
 * Plugins can process frames, draw overlays and react to video events.
 * 
 * Design for high performance:
 * - Pipeline processing
 * - In-place modification when possible
 * - Enable control to avoid overhead
 */
class IVideoPlugin
{
public:
    virtual ~IVideoPlugin() = default;

    /**
     * @brief Processes a video frame
     * 
     * This method is called for each video frame. The plugin can:
     * - Modify the frame directly (draw, apply filters, etc)
     * - Extract information from the frame (detection, analysis, etc)
     * 
     * @param frame Video frame (can be modified)
     * @param timestamp Frame timestamp in milliseconds
     * @param frameIndex Current frame index
     * @return true if processing was successful, false otherwise
     */
    virtual bool processFrame(cv::Mat& frame, qint64 timestamp, qint64 frameIndex) = 0;

    /**
     * @brief Initializes the plugin with video information
     * 
     * Called when a new video is loaded
     * 
     * @param videoInfo Video information (width, height, fps, duration, etc)
     */
    virtual void initialize(const QVariantMap& videoInfo) = 0;

    /**
     * @brief Finalizes the plugin
     * 
     * Called when the video is unloaded or the plugin is removed
     */
    virtual void finalize() = 0;

    /**
     * @brief Called when video starts playback
     */
    virtual void onPlaybackStarted() {}

    /**
     * @brief Called when video is paused
     */
    virtual void onPlaybackPaused() {}

    /**
     * @brief Called when video is stopped
     */
    virtual void onPlaybackStopped() {}

    /**
     * @brief Called when there is a seek in the video
     * @param position New position in milliseconds
     */
    virtual void onSeek(qint64 position) {}

    /**
     * @brief Returns the plugin name
     */
    virtual QString getName() const = 0;

    /**
     * @brief Returns the plugin version
     */
    virtual QString getVersion() const = 0;

    /**
     * @brief Returns the plugin description
     */
    virtual QString getDescription() const = 0;

    /**
     * @brief Checks if the plugin is enabled
     */
    virtual bool isEnabled() const = 0;

    /**
     * @brief Enables or disables the plugin
     * @param enabled true to enable, false to disable
     */
    virtual void setEnabled(bool enabled) = 0;

    /**
     * @brief Sets plugin settings
     * @param settings Settings map
     */
    virtual void setSettings(const QVariantMap& settings) {}

    /**
     * @brief Gets plugin settings
     * @return Settings map
     */
    virtual QVariantMap getSettings() const { return QVariantMap(); }

    /**
     * @brief Plugin execution priority (lower = executes first)
     * 
     * Plugins with lower priority are executed first.
     * Useful for controlling processing order.
     * 
     * @return Priority value (default: 100)
     */
    virtual int getPriority() const { return 100; }
};

#endif // IVIDEOPLUGIN_H
