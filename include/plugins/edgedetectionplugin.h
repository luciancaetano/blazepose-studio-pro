#ifndef EDGEDETECTIONPLUGIN_H
#define EDGEDETECTIONPLUGIN_H

#include "ivideoplugin.h"
#include <opencv2/opencv.hpp>

/**
 * @brief Example plugin that applies edge detection (Canny)
 * 
 * This plugin demonstrates:
 * - Image processing with OpenCV
 * - Adjustable settings
 * - Visualization mode (original, edges, or blend)
 */
class EdgeDetectionPlugin : public IVideoPlugin
{
public:
    enum class ViewMode {
        Original,   // Show original video
        Edges,      // Show only edges
        Blend       // Blend edges with original
    };

    EdgeDetectionPlugin();
    virtual ~EdgeDetectionPlugin() = default;

    // IVideoPlugin interface
    bool processFrame(cv::Mat& frame, qint64 timestamp, qint64 frameIndex) override;
    void initialize(const QVariantMap& videoInfo) override;
    void finalize() override;
    
    QString getName() const override;
    QString getVersion() const override;
    QString getDescription() const override;
    
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    void setSettings(const QVariantMap& settings) override;
    QVariantMap getSettings() const override;
    
    int getPriority() const override;

    // Specific settings
    void setThresholds(int low, int high);
    void setViewMode(ViewMode mode);
    void setBlendAlpha(double alpha);
    void setEdgeColor(const cv::Scalar& color);

private:
    void detectEdges(const cv::Mat& input, cv::Mat& output);
    void applyColorToEdges(cv::Mat& edges);

    bool m_enabled;
    
    // Detection parameters
    int m_lowThreshold;
    int m_highThreshold;
    
    // Visualization mode
    ViewMode m_viewMode;
    double m_blendAlpha;  // For Blend mode (0.0 - 1.0)
    
    // Edge color (in color mode)
    cv::Scalar m_edgeColor;
    bool m_useColorEdges;
};

#endif // EDGEDETECTIONPLUGIN_H
