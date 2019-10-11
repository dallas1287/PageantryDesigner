#include "DotsRenderer.h"

DotsRenderer::DotsRenderer(GraphicsPanel* parent) : ItemRenderer(parent)
{
}

DotsRenderer::~DotsRenderer()
{
	delete m_texture;
	delete m_program;
}

void DotsRenderer::initialize()
{
	initShaders();
	initTextures("../flag.png");
}

void DotsRenderer::initShaders()
{
	m_program = new QOpenGLShaderProgram;
	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "dots_vertex.glsl"))
		qDebug() << m_program->log();
	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "dots_fragment.glsl"))
		qDebug() << m_program->log();

	if (!m_program->link())
		qDebug() << m_program->log();

}

void DotsRenderer::initTextures(const QString& path)
{
	//OpenGL flips the image so it needs to be mirrored when created
	QImage image(path);
	m_texture = new QOpenGLTexture(image.mirrored());

	// Set nearest filtering mode for texture minification
	m_texture->setMinificationFilter(QOpenGLTexture::Nearest);

	// Set bilinear filtering mode for texture magnification
	m_texture->setMagnificationFilter(QOpenGLTexture::Linear);

	// Wrap texture coordinates by repeating
	m_texture->setWrapMode(QOpenGLTexture::Repeat);
}

void DotsRenderer::setupAttributes()
{
	m_posAttr = m_program->attributeLocation("posAttr");
	m_texCoordAttr = m_program->attributeLocation("texCoordAttr");
	m_colAttr = m_program->attributeLocation("colAttr");
	m_matrixUniform = m_program->uniformLocation("matrix");
}

void DotsRenderer::Draw()
{

}