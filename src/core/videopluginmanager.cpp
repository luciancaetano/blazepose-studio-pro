#include "core/videopluginmanager.h"
#include <QDebug>
#include <algorithm>

VideoPluginManager::VideoPluginManager(QObject *parent)
    : QObject(parent)
    , m_needsSort(false)
{
    qDebug() << "[VideoPluginManager] Initialized";
}

VideoPluginManager::~VideoPluginManager()
{
    finalizePlugins();
    qDebug() << "[VideoPluginManager] Destroyed";
}

void VideoPluginManager::addPlugin(std::shared_ptr<IVideoPlugin> plugin)
{
    if (!plugin) {
        qWarning() << "[VideoPluginManager] Attempt to add null plugin";
        return;
    }

    QString pluginName = plugin->getName();

    // Check if plugin with same name already exists
    for (const auto& existingPlugin : m_plugins) {
        if (existingPlugin->getName() == pluginName) {
            qWarning() << "[VideoPluginManager] Plugin already exists:" << pluginName;
            return;
        }
    }

    m_plugins.append(plugin);
    m_needsSort = true;

    qDebug() << "[VideoPluginManager] Plugin added:" << pluginName
             << "Version:" << plugin->getVersion()
             << "Priority:" << plugin->getPriority();

    emit pluginAdded(pluginName);
    updateEnabledPluginsCache();
}

bool VideoPluginManager::removePlugin(const QString& pluginName)
{
    for (int i = 0; i < m_plugins.size(); ++i) {
        if (m_plugins[i]->getName() == pluginName) {
            m_plugins[i]->finalize();
            m_plugins.removeAt(i);
            qDebug() << "[VideoPluginManager] Plugin removed:" << pluginName;
            emit pluginRemoved(pluginName);
            updateEnabledPluginsCache();
            return true;
        }
    }

    qWarning() << "[VideoPluginManager] Plugin not found:" << pluginName;
    return false;
}

void VideoPluginManager::removeAllPlugins()
{
    qDebug() << "[VideoPluginManager] Removing all plugins";
    finalizePlugins();
    m_plugins.clear();
    m_enabledPlugins.clear();
}

std::shared_ptr<IVideoPlugin> VideoPluginManager::getPlugin(const QString& pluginName)
{
    for (const auto& plugin : m_plugins) {
        if (plugin->getName() == pluginName) {
            return plugin;
        }
    }
    return nullptr;
}

QList<std::shared_ptr<IVideoPlugin>> VideoPluginManager::getAllPlugins() const
{
    return m_plugins;
}

bool VideoPluginManager::processFrame(cv::Mat& frame, qint64 timestamp, qint64 frameIndex)
{
    if (frame.empty()) {
        return false;
    }

    // Ordenar plugins se necessário
    if (m_needsSort) {
        sortPluginsByPriority();
        m_needsSort = false;
    }

    // Processar apenas plugins habilitados (usando cache)
    bool allSuccess = true;
    for (const auto& plugin : m_enabledPlugins) {
        try {
            if (!plugin->processFrame(frame, timestamp, frameIndex)) {
                qWarning() << "[VideoPluginManager] Plugin failed processing:"
                          << plugin->getName();
                allSuccess = false;
            }
        } catch (const std::exception& e) {
            qWarning() << "[VideoPluginManager] Exception in plugin" << plugin->getName()
                      << ":" << e.what();
            allSuccess = false;
        }
    }

    return allSuccess;
}

void VideoPluginManager::initializePlugins(const QVariantMap& videoInfo)
{
    qDebug() << "[VideoPluginManager] Initializing" << m_plugins.size() << "plugins";
    
    for (const auto& plugin : m_plugins) {
        try {
            plugin->initialize(videoInfo);
            qDebug() << "[VideoPluginManager] Plugin initialized:" << plugin->getName();
        } catch (const std::exception& e) {
            qWarning() << "[VideoPluginManager] Error initializing plugin"
                      << plugin->getName() << ":" << e.what();
        }
    }

    updateEnabledPluginsCache();
}

void VideoPluginManager::finalizePlugins()
{
    qDebug() << "[VideoPluginManager] Finalizing" << m_plugins.size() << "plugins";
    
    for (const auto& plugin : m_plugins) {
        try {
            plugin->finalize();
            qDebug() << "[VideoPluginManager] Plugin finalized:" << plugin->getName();
        } catch (const std::exception& e) {
            qWarning() << "[VideoPluginManager] Error finalizing plugin"
                      << plugin->getName() << ":" << e.what();
        }
    }

    m_enabledPlugins.clear();
}

void VideoPluginManager::notifyPlaybackStarted()
{
    for (const auto& plugin : m_enabledPlugins) {
        plugin->onPlaybackStarted();
    }
}

void VideoPluginManager::notifyPlaybackPaused()
{
    for (const auto& plugin : m_enabledPlugins) {
        plugin->onPlaybackPaused();
    }
}

void VideoPluginManager::notifyPlaybackStopped()
{
    for (const auto& plugin : m_enabledPlugins) {
        plugin->onPlaybackStopped();
    }
}

void VideoPluginManager::notifySeek(qint64 position)
{
    for (const auto& plugin : m_enabledPlugins) {
        plugin->onSeek(position);
    }
}

bool VideoPluginManager::setPluginEnabled(const QString& pluginName, bool enabled)
{
    auto plugin = getPlugin(pluginName);
    if (!plugin) {
        return false;
    }

    plugin->setEnabled(enabled);
    updateEnabledPluginsCache();
    
    qDebug() << "[VideoPluginManager] Plugin" << pluginName
             << (enabled ? "enabled" : "disabled");
    
    emit pluginStateChanged(pluginName, enabled);
    return true;
}

int VideoPluginManager::getPluginCount() const
{
    return m_plugins.size();
}

int VideoPluginManager::getEnabledPluginCount() const
{
    return m_enabledPlugins.size();
}

void VideoPluginManager::sortPluginsByPriority()
{
    std::sort(m_plugins.begin(), m_plugins.end(),
              [](const std::shared_ptr<IVideoPlugin>& a, const std::shared_ptr<IVideoPlugin>& b) {
                  return a->getPriority() < b->getPriority();
              });
}

void VideoPluginManager::updateEnabledPluginsCache()
{
    m_enabledPlugins.clear();
    
    for (const auto& plugin : m_plugins) {
        if (plugin->isEnabled()) {
            m_enabledPlugins.append(plugin);
        }
    }

    qDebug() << "[VideoPluginManager] Cache updated:"
             << m_enabledPlugins.size() << "enabled plugins out of"
             << m_plugins.size() << "total";
}
