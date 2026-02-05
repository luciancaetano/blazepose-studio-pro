#include "plugins/edgedetectionplugin.h"
#include <QDebug>
#include <QColor>

EdgeDetectionPlugin::EdgeDetectionPlugin()
    : m_enabled(false)  // Disabled by default (heavy effect)
    , m_lowThreshold(50)
    , m_highThreshold(150)
    , m_viewMode(ViewMode::Blend)
    , m_blendAlpha(0.5)
    , m_edgeColor(0, 255, 0)  // Green
    , m_useColorEdges(true)
{
}

bool EdgeDetectionPlugin::processFrame(cv::Mat& frame, qint64 timestamp, qint64 frameIndex)
{
    if (!m_enabled || frame.empty()) {
        return false;
    }

    cv::Mat edges;
    detectEdges(frame, edges);

    switch (m_viewMode) {
        case ViewMode::Original:
            // Do nothing, keep original
            break;

        case ViewMode::Edges:
            // Show only edges
            if (m_useColorEdges) {
                applyColorToEdges(edges);
            }
            // Convert edges to RGB if necessary
            if (edges.channels() == 1) {
                cv::cvtColor(edges, frame, cv::COLOR_GRAY2RGB);
            } else {
                frame = edges;
            }
            break;

        case ViewMode::Blend:
            // Blend edges with original image
            if (m_useColorEdges) {
                applyColorToEdges(edges);
            } else {
                cv::cvtColor(edges, edges, cv::COLOR_GRAY2RGB);
            }
            cv::addWeighted(frame, 1.0 - m_blendAlpha, edges, m_blendAlpha, 0, frame);
            break;
    }

    return true;
}

void EdgeDetectionPlugin::initialize(const QVariantMap& videoInfo)
{
    int width = videoInfo.value("width", 0).toInt();
    int height = videoInfo.value("height", 0).toInt();
    
    qDebug() << "[EdgeDetectionPlugin] Initialized for video"
             << width << "x" << height;
}

void EdgeDetectionPlugin::finalize()
{
    qDebug() << "[EdgeDetectionPlugin] Finalized";
}

QString EdgeDetectionPlugin::getName() const
{
    return "Edge Detection Plugin";
}

QString EdgeDetectionPlugin::getVersion() const
{
    return "1.0.0";
}

QString EdgeDetectionPlugin::getDescription() const
{
    return "Detects and visualizes edges using Canny algorithm";
}

bool EdgeDetectionPlugin::isEnabled() const
{
    return m_enabled;
}

void EdgeDetectionPlugin::setEnabled(bool enabled)
{
    m_enabled = enabled;
    qDebug() << "[EdgeDetectionPlugin]" << (enabled ? "Enabled" : "Disabled");
}

void EdgeDetectionPlugin::setSettings(const QVariantMap& settings)
{
    m_lowThreshold = settings.value("lowThreshold", 50).toInt();
    m_highThreshold = settings.value("highThreshold", 150).toInt();
    m_blendAlpha = settings.value("blendAlpha", 0.5).toDouble();
    m_useColorEdges = settings.value("useColorEdges", true).toBool();
    
    // ViewMode como string
    QString modeStr = settings.value("viewMode", "blend").toString().toLower();
    if (modeStr == "original") {
        m_viewMode = ViewMode::Original;
    } else if (modeStr == "edges") {
        m_viewMode = ViewMode::Edges;
    } else {
        m_viewMode = ViewMode::Blend;
    }
    
    // Cor das bordas
    if (settings.contains("edgeColor")) {
        QColor color = settings.value("edgeColor").value<QColor>();
        m_edgeColor = cv::Scalar(color.blue(), color.green(), color.red());
    }
}

QVariantMap EdgeDetectionPlugin::getSettings() const
{
    QVariantMap settings;
    settings["lowThreshold"] = m_lowThreshold;
    settings["highThreshold"] = m_highThreshold;
    settings["blendAlpha"] = m_blendAlpha;
    settings["useColorEdges"] = m_useColorEdges;
    
    QString modeStr;
    switch (m_viewMode) {
        case ViewMode::Original: modeStr = "original"; break;
        case ViewMode::Edges: modeStr = "edges"; break;
        case ViewMode::Blend: modeStr = "blend"; break;
    }
    settings["viewMode"] = modeStr;
    
    QColor color(m_edgeColor[2], m_edgeColor[1], m_edgeColor[0]);
    settings["edgeColor"] = color;
    
    return settings;
}

int EdgeDetectionPlugin::getPriority() const
{
    return 200; // Execute before overlays, but after preprocessing
}

void EdgeDetectionPlugin::setThresholds(int low, int high)
{
    m_lowThreshold = qBound(0, low, 255);
    m_highThreshold = qBound(0, high, 255);
    
    // Ensure that high >= low
    if (m_highThreshold < m_lowThreshold) {
        std::swap(m_lowThreshold, m_highThreshold);
    }
    
    qDebug() << "[EdgeDetectionPlugin] Thresholds:" << m_lowThreshold << "-" << m_highThreshold;
}

void EdgeDetectionPlugin::setViewMode(ViewMode mode)
{
    m_viewMode = mode;
}

void EdgeDetectionPlugin::setBlendAlpha(double alpha)
{
    m_blendAlpha = qBound(0.0, alpha, 1.0);
}

void EdgeDetectionPlugin::setEdgeColor(const cv::Scalar& color)
{
    m_edgeColor = color;
    m_useColorEdges = true;
}

void EdgeDetectionPlugin::detectEdges(const cv::Mat& input, cv::Mat& output)
{
    // Convert to grayscale
    cv::Mat gray;
    if (input.channels() == 3) {
        cv::cvtColor(input, gray, cv::COLOR_RGB2GRAY);
    } else if (input.channels() == 4) {
        cv::cvtColor(input, gray, cv::COLOR_RGBA2GRAY);
    } else {
        gray = input.clone();
    }
    
    // Apply blur to reduce noise
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 1.4);
    
    // Detect edges with Canny
    cv::Canny(gray, output, m_lowThreshold, m_highThreshold);
}

void EdgeDetectionPlugin::applyColorToEdges(cv::Mat& edges)
{
    // Convert edges to RGB
    cv::Mat colorEdges;
    cv::cvtColor(edges, colorEdges, cv::COLOR_GRAY2RGB);
    
    // Apply custom color
    for (int i = 0; i < colorEdges.rows; i++) {
        for (int j = 0; j < colorEdges.cols; j++) {
            cv::Vec3b& pixel = colorEdges.at<cv::Vec3b>(i, j);
            if (pixel[0] > 0) {  // If is an edge (white)
                pixel[0] = m_edgeColor[0];
                pixel[1] = m_edgeColor[1];
                pixel[2] = m_edgeColor[2];
            }
        }
    }
    
    edges = colorEdges;
}
