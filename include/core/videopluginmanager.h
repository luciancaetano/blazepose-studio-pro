#ifndef VIDEOPLUGINMANAGER_H
#define VIDEOPLUGINMANAGER_H

#include "ivideoplugin.h"
#include <QObject>
#include <QList>
#include <memory>
#include <opencv2/opencv.hpp>

/**
 * @brief Video plugin manager
 * 
 * Manages the lifecycle and execution of video plugins.
 * Plugins are executed in priority order.
 * 
 * Design for performance:
 * - Optimized sequential processing
 * - Enabled plugins cache
 * - Conditional execution based on state
 */
class VideoPluginManager : public QObject
{
    Q_OBJECT

public:
    explicit VideoPluginManager(QObject *parent = nullptr);
    ~VideoPluginManager();

    /**
     * @brief Adds a plugin to the manager
     * @param plugin Plugin to be added (manager takes ownership)
     */
    void addPlugin(std::shared_ptr<IVideoPlugin> plugin);

    /**
     * @brief Removes a plugin by name
     * @param pluginName Name of the plugin to be removed
     * @return true if the plugin was removed, false otherwise
     */
    bool removePlugin(const QString& pluginName);

    /**
     * @brief Removes all plugins
     */
    void removeAllPlugins();

    /**
     * @brief Gets a plugin by name
     * @param pluginName Plugin name
     * @return Pointer to the plugin or nullptr if not found
     */
    std::shared_ptr<IVideoPlugin> getPlugin(const QString& pluginName);

    /**
     * @brief Gets list of all plugins
     * @return List of plugins
     */
    QList<std::shared_ptr<IVideoPlugin>> getAllPlugins() const;

    /**
     * @brief Processes a frame through all enabled plugins
     * 
     * Plugins are executed in priority order (lower first)
     * 
     * @param frame Frame to be processed (modified in-place)
     * @param timestamp Frame timestamp in milliseconds
     * @param frameIndex Frame index
     * @return true if all plugins processed successfully
     */
    bool processFrame(cv::Mat& frame, qint64 timestamp, qint64 frameIndex);

    /**
     * @brief Initializes all plugins with video information
     * @param videoInfo Video information
     */
    void initializePlugins(const QVariantMap& videoInfo);

    /**
     * @brief Finalizes all plugins
     */
    void finalizePlugins();

    /**
     * @brief Notifies plugins that playback has started
     */
    void notifyPlaybackStarted();

    /**
     * @brief Notifies plugins that playback was paused
     */
    void notifyPlaybackPaused();

    /**
     * @brief Notifies plugins that playback was stopped
     */
    void notifyPlaybackStopped();

    /**
     * @brief Notifies plugins about a seek
     * @param position New position in milliseconds
     */
    void notifySeek(qint64 position);

    /**
     * @brief Enables or disables a plugin by name
     * @param pluginName Plugin name
     * @param enabled true to enable, false to disable
     * @return true if the plugin was found and modified
     */
    bool setPluginEnabled(const QString& pluginName, bool enabled);

    /**
     * @brief Gets the number of plugins
     * @return Total number of plugins
     */
    int getPluginCount() const;

    /**
     * @brief Gets the number of enabled plugins
     * @return Number of enabled plugins
     */
    int getEnabledPluginCount() const;

signals:
    /**
     * @brief Emitted when a plugin is added
     * @param pluginName Plugin name
     */
    void pluginAdded(const QString& pluginName);

    /**
     * @brief Emitted when a plugin is removed
     * @param pluginName Plugin name
     */
    void pluginRemoved(const QString& pluginName);

    /**
     * @brief Emitted when a plugin is enabled/disabled
     * @param pluginName Plugin name
     * @param enabled Plugin state
     */
    void pluginStateChanged(const QString& pluginName, bool enabled);

private:
    void sortPluginsByPriority();
    void updateEnabledPluginsCache();

    QList<std::shared_ptr<IVideoPlugin>> m_plugins;
    QList<std::shared_ptr<IVideoPlugin>> m_enabledPlugins; // Cache for performance
    bool m_needsSort;
};

#endif // VIDEOPLUGINMANAGER_H
