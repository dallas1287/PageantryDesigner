#include "ShadowMap.h"

ShadowMap::ShadowMap()
{
	initializeOpenGLFunctions();
}

ShadowMap::~ShadowMap()
{
}

QOpenGLVertexArrayObject& ShadowMap::Vao()
{
	if (!m_vao.isCreated())
		m_vao.create();
	return m_vao;
}

QOpenGLBuffer ShadowMap::Vbo()
{
	if (!m_vbo.isCreated())
		m_vbo.create();
	return m_vbo;
}

void ShadowMap::initShader()
{
	m_program.reset(new QOpenGLShaderProgram);
	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADOW_MAP_VS))
		qDebug() << m_program->log();
	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADOW_MAP_FRAG))
		qDebug() << m_program->log();
	if (!m_program->link())
	{
		qDebug() << m_program->log();
		return;
	}
	m_posAttr = m_program->attributeLocation("posAttr");
}

void ShadowMap::initDepthMap()
{
	//creates an object that can be bound later to a GL_TEXTURE_2D
	m_depthMapTexture.reset(new QOpenGLTexture(QOpenGLTexture::Target::Target2D));
	m_depthMapTexture->setMinificationFilter(QOpenGLTexture::Nearest);
	m_depthMapTexture->setMagnificationFilter(QOpenGLTexture::Nearest);
	m_depthMapTexture->setWrapMode(QOpenGLTexture::Repeat);
}

void ShadowMap::initFrameBuffer()
{
	m_fbo.reset(new QOpenGLFramebufferObject(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, QOpenGLFramebufferObject::Attachment::Depth));
	m_fbo->bind();
	//attach texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMapTexture->textureId(), 0);
	//this differs from the example in LearOpenGL in that it calls glDrawBuffer(GL_NONE) which doesn't seem to be available in the 
	//qt OpenGLExtraFunctions 
	GLenum buf = GL_NONE;
	glDrawBuffers(1, &buf);
	//glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	m_fbo->release();
}

void ShadowMap::bindToDraw()
{
	if (!m_program)
		return;
	m_program->bind();

	if (m_depthMapTexture)
	{
		glActiveTexture(GL_TEXTURE6);
		m_depthMapTexture->bind(GL_TEXTURE6);
		m_program->setUniformValue("depthMap", GL_TEXTURE6 - GL_TEXTURE0);
	}
}

void ShadowMap::setViewport()
{
	glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
}