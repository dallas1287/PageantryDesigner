#include "GraphicsObject.h"

GraphicsObject::GraphicsObject() : m_ebo(QOpenGLBuffer(QOpenGLBuffer::IndexBuffer))
{
	//initialize arrays and attributes
	Vao();
	Vbo();
	Ebo();
}

GraphicsObject::~GraphicsObject()
{
}

QOpenGLVertexArrayObject& GraphicsObject::Vao()
{
	if (!m_vao.isCreated())
		m_vao.create();
	return m_vao;
}

QOpenGLBuffer GraphicsObject::Vbo()
{
	if (!m_vbo.isCreated())
		m_vbo.create();
	return m_vbo;
}

QOpenGLBuffer GraphicsObject::Ebo()
{
	if (!m_ebo.isCreated())
		m_ebo.create();
	return m_ebo;
}

void GraphicsObject::initShaders(const QString& vertexPath, const QString& fragmentPath)
{
	m_program.reset(new QOpenGLShaderProgram);
	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexPath))
		qDebug() << m_program->log();
	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentPath))
		qDebug() << m_program->log();
	if (!m_program->link())
	{
		qDebug() << m_program->log();
		return;
	}
	setupAttributes();
}

void GraphicsObject::initTexture(const QString& path)
{
	//OpenGL flips the image so it needs to be mirrored when created
	m_texture.reset(new QOpenGLTexture(QImage(path).mirrored()));

	// Set nearest filtering mode for texture minification
	m_texture->setMinificationFilter(QOpenGLTexture::Nearest);

	// Set bilinear filtering mode for texture magnification
	m_texture->setMagnificationFilter(QOpenGLTexture::Linear);

	// Wrap texture coordinates by repeating
	m_texture->setWrapMode(QOpenGLTexture::Repeat);
}

void GraphicsObject::setupAttributes()
{
	m_shaderAttributes.m_posAttr = m_program->attributeLocation("posAttr");
	m_shaderAttributes.m_texCoordAttr = m_program->attributeLocation("texCoordAttr");
	m_shaderAttributes.m_colAttr = m_program->attributeLocation("colAttr");
	m_shaderAttributes.m_normalAttr = m_program->attributeLocation("normAttr");
	m_shaderAttributes.m_boneTransform0 = m_program->attributeLocation("boneTransform0");
	m_shaderAttributes.m_boneTransform1 = m_program->attributeLocation("boneTransform1");
	m_shaderAttributes.m_boneTransform2 = m_program->attributeLocation("boneTransform2");
	m_shaderAttributes.m_boneTransform3 = m_program->attributeLocation("boneTransform3");
	m_shaderAttributes.m_modelUniform = m_program->uniformLocation("model");
	m_shaderAttributes.m_viewUniform = m_program->uniformLocation("view");
	m_shaderAttributes.m_projectionUniform = m_program->uniformLocation("projection");
}

void GraphicsObject::setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection)
{
	m_program->bind();
	m_program->setUniformValue(ModelAttr(), model);
	m_program->setUniformValue(ViewAttr(), view);
	m_program->setUniformValue(ProjAttr(), projection);
	m_program->release();
}

void GraphicsObject::bindAll()
{
	m_program->bind();
	m_vao.bind();
	m_vbo.bind();
	m_ebo.bind();
}

void GraphicsObject::releaseAll()
{
	m_vao.release();
	m_ebo.release();
	m_vbo.release();
	m_program->release();
}

void GraphicsObject::bindToDraw(bool useTexture)
{
	m_program->bind();
	if(useTexture)
		m_texture->bind();
	m_vao.bind();
}

void GraphicsObject::releaseFromDraw()
{
	m_vao.release();
	if(m_texture && m_texture->isBound())
		m_texture->release();
	m_program->release();
}