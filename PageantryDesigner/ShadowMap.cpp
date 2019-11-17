#include "ShadowMap.h"
#include "utils.h"

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
	m_depthMapTexture->setFormat(QOpenGLTexture::D16);
	m_depthMapTexture->setDepthStencilMode(QOpenGLTexture::DepthMode);
	m_depthMapTexture->setSize(1024, 1024);
	m_depthMapTexture->setMinificationFilter(QOpenGLTexture::Nearest);
	m_depthMapTexture->setMagnificationFilter(QOpenGLTexture::Nearest);
	m_depthMapTexture->setWrapMode(QOpenGLTexture::Repeat);
	m_depthMapTexture->allocateStorage();
	m_depthMapTexture->bind();
}

void ShadowMap::initFrameBuffer()
{
	m_fbo.reset(new QOpenGLFramebufferObject(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, QOpenGLFramebufferObject::Attachment::Depth));
	m_fbo->bind();

	if (!m_depthMapTexture || !m_depthMapTexture->isCreated())
		initDepthMap();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMapTexture->textureId(), 0);
	//this differs from the example in LearOpenGL in that it calls glDrawBuffer(GL_NONE) which doesn't seem to be available in the 
	//qt OpenGLExtraFunctions 
	GLenum buf = GL_NONE;
	glDrawBuffers(1, &buf);
	//glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		qDebug() << "OpenGL Frambuffer status not complete.";
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

void ShadowMap::setLightSpaceMatrix(const QVector3D& lightPos)
{
	m_projection.ortho(-10.0, 10.0, -10.0, 10.0, m_nearPlane, m_farPlane);
	m_view.setToIdentity();
	m_view.lookAt(lightPos, QVector3D(0.0, 0.0, 0.0), Y);
	m_lightSpaceMatrix = m_projection * m_view;
	setShaderLightSpaceMatrix();
}

void ShadowMap::setShaderLightSpaceMatrix()
{
	if (!m_program)
		return;
	m_program->bind();
	m_program->setUniformValue("lightSpaceMatrix", m_lightSpaceMatrix);
	m_program->release();
}

void ShadowMap::setModelUniform(const QMatrix4x4& model)
{
	if (!m_program)
		return;
	m_program->bind();
	m_program->setUniformValue("model", m_lightSpaceMatrix);
	m_program->release();
}

void ShadowMap::saveBufferAsImage()
{
	if (m_fbo->isValid())
	{
		QImage image = m_fbo->toImage();
		image.save("../bufferOutput.png");
	}
	else 
		qDebug() << "INVALID FrameBuffer: Cant Save image";
}