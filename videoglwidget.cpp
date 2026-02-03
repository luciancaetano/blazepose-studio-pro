#include "videoglwidget.h"
#include <QDebug>
#include <QSurfaceFormat>

VideoGLWidget::VideoGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_textureId(0)
    , m_hasFrame(false)
    , m_glInitialized(false)
{
    qDebug() << "[VideoGLWidget] Construtor chamado";
    setMinimumSize(320, 240);
    
    // Configurar formato OpenGL para usar perfil de compatibilidade
    QSurfaceFormat format;
    format.setVersion(2, 1);  // OpenGL 2.1
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    setFormat(format);
    qDebug() << "[VideoGLWidget] Formato OpenGL configurado: OpenGL 2.1 Compatibility";
}

VideoGLWidget::~VideoGLWidget()
{
    qDebug() << "[VideoGLWidget] Destrutor chamado";
    makeCurrent();
    deleteTexture();
    doneCurrent();
}

void VideoGLWidget::initializeGL()
{
    qDebug() << "[VideoGLWidget] initializeGL chamado";
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    m_glInitialized = true;
    qDebug() << "[VideoGLWidget] OpenGL inicializado - GL_TEXTURE_2D habilitado";
    
    // Se já temos um frame esperando, criar a textura agora
    if (m_hasFrame && !m_currentFrame.empty()) {
        qDebug() << "[VideoGLWidget] Frame pendente detectado, criando textura agora";
        createTexture();
    }
}

void VideoGLWidget::resizeGL(int w, int h)
{
    qDebug() << "[VideoGLWidget] resizeGL chamado - tamanho:" << w << "x" << h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void VideoGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    qDebug() << "[VideoGLWidget] paintGL - hasFrame:" << m_hasFrame << "textureId:" << m_textureId;
    
    if (!m_hasFrame || m_textureId == 0) {
        qDebug() << "[VideoGLWidget] paintGL - Não há frame ou textura para renderizar";
        return;
    }

    qDebug() << "[VideoGLWidget] paintGL - Renderizando textura...";
    glLoadIdentity();
    
    // Calcular proporção para manter aspect ratio da imagem
    float imgAspect = static_cast<float>(m_currentFrame.cols) / m_currentFrame.rows;
    float widgetAspect = static_cast<float>(width()) / height();
    
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    
    if (widgetAspect > imgAspect) {
        scaleX = imgAspect / widgetAspect;
    } else {
        scaleY = widgetAspect / imgAspect;
    }
    
    qDebug() << "[VideoGLWidget] paintGL - Aspect ratio - img:" << imgAspect << "widget:" << widgetAspect;
    qDebug() << "[VideoGLWidget] paintGL - Scale:" << scaleX << "x" << scaleY;

    glBindTexture(GL_TEXTURE_2D, m_textureId);
    
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-scaleX, -scaleY);
    glTexCoord2f(1.0f, 1.0f); glVertex2f( scaleX, -scaleY);
    glTexCoord2f(1.0f, 0.0f); glVertex2f( scaleX,  scaleY);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-scaleX,  scaleY);
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, 0);
    qDebug() << "[VideoGLWidget] paintGL - Renderização concluída";
}

void VideoGLWidget::updateFrame(const cv::Mat& frame)
{
    qDebug() << "[VideoGLWidget] updateFrame chamado";
    
    if (frame.empty()) {
        qWarning() << "[VideoGLWidget] Frame vazio recebido!";
        return;
    }
    
    qDebug() << "[VideoGLWidget] Frame recebido - tamanho:" << frame.cols << "x" << frame.rows 
             << "canais:" << frame.channels() << "tipo:" << frame.type();

    // Converter para RGB se necessário
    cv::Mat rgbFrame;
    if (frame.channels() == 1) {
        qDebug() << "[VideoGLWidget] Convertendo GRAY para RGB";
        cv::cvtColor(frame, rgbFrame, cv::COLOR_GRAY2RGB);
    } else if (frame.channels() == 3) {
        qDebug() << "[VideoGLWidget] Convertendo BGR para RGB";
        cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);
    } else if (frame.channels() == 4) {
        qDebug() << "[VideoGLWidget] Convertendo BGRA para RGB";
        cv::cvtColor(frame, rgbFrame, cv::COLOR_BGRA2RGB);
    } else {
        qDebug() << "[VideoGLWidget] Usando frame direto (sem conversão)";
        rgbFrame = frame.clone();
    }

    m_currentFrame = rgbFrame;
    m_hasFrame = true;
    
    qDebug() << "[VideoGLWidget] Frame processado - tamanho final:" << m_currentFrame.cols << "x" << m_currentFrame.rows;

    // Só criar textura se OpenGL já foi inicializado
    if (m_glInitialized) {
        qDebug() << "[VideoGLWidget] OpenGL já inicializado, criando textura agora";
        makeCurrent();
        createTexture();
        doneCurrent();
    } else {
        qDebug() << "[VideoGLWidget] OpenGL ainda não inicializado, textura será criada no initializeGL";
    }
    
    qDebug() << "[VideoGLWidget] Chamando update() para redesenhar";
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
    qDebug() << "[VideoGLWidget] createTexture chamado";
    
    if (m_currentFrame.empty()) {
        qWarning() << "[VideoGLWidget] currentFrame vazio, não pode criar textura";
        return;
    }

    deleteTexture();
    
    qDebug() << "[VideoGLWidget] Gerando textura OpenGL...";

    // Gerar textura OpenGL
    glGenTextures(1, &m_textureId);
    qDebug() << "[VideoGLWidget] Textura gerada com ID:" << m_textureId;
    
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    
    // Configurar parâmetros da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    qDebug() << "[VideoGLWidget] Carregando dados da imagem na textura...";
    qDebug() << "[VideoGLWidget] Tamanho:" << m_currentFrame.cols << "x" << m_currentFrame.rows;
    qDebug() << "[VideoGLWidget] Ponteiro de dados:" << (void*)m_currentFrame.data;
    qDebug() << "[VideoGLWidget] isContinuous:" << m_currentFrame.isContinuous();
    
    // Carregar dados da imagem na textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 m_currentFrame.cols, m_currentFrame.rows,
                 0, GL_RGB, GL_UNSIGNED_BYTE,
                 m_currentFrame.data);
    
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        qWarning() << "[VideoGLWidget] Erro OpenGL após glTexImage2D:" << error;
    } else {
        qDebug() << "[VideoGLWidget] Textura criada com sucesso!";
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void VideoGLWidget::deleteTexture()
{
    if (m_textureId != 0) {
        qDebug() << "[VideoGLWidget] Deletando textura ID:" << m_textureId;
        glDeleteTextures(1, &m_textureId);
        m_textureId = 0;
    }
}
