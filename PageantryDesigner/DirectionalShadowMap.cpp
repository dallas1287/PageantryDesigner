#include "DirectionalShadowMap.h"

DirectionalShadowMap::DirectionalShadowMap()
{
	m_type = ShadowType::Directional;
	m_nearPlane = 1.0;
	m_farPlane = 7.5;
}

DirectionalShadowMap::~DirectionalShadowMap()
{
}

void DirectionalShadowMap::initDepthMap()
{
	//creates an object that can be bound later to a GL_TEXTURE_2D
	m_depthMapTexture.reset(new QOpenGLTexture(QOpenGLTexture::Target::Target2D));
	m_depthMapTexture->setFormat(QOpenGLTexture::DepthFormat);
	m_depthMapTexture->setDepthStencilMode(QOpenGLTexture::DepthMode);
	m_depthMapTexture->setSize(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
	m_depthMapTexture->setMinificationFilter(QOpenGLTexture::Nearest);
	m_depthMapTexture->setMagnificationFilter(QOpenGLTexture::Nearest);
	m_depthMapTexture->setWrapMode(QOpenGLTexture::Repeat);
	m_depthMapTexture->allocateStorage();
	m_depthMapTexture->bind();
}

void DirectionalShadowMap::initFrameBuffer()
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
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		qDebug() << "OpenGL Frambuffer status not complete.";

	m_fbo->release();
}

void DirectionalShadowMap::setLightSpaceMatrix(const QVector3D& lightPos)
{
	m_projection.setToIdentity();
	m_projection.ortho(-10.0, 10.0, -10.0, 10.0, m_nearPlane, m_farPlane);
	m_view.setToIdentity();
	m_view.lookAt(lightPos, QVector3D(), Y);
	m_lightSpaceMatrix = m_projection * m_view;
	setShaderLightSpaceMatrix();
}

void DirectionalShadowMap::setShaderLightSpaceMatrix()
{
	if (!ShaderProgram())
		return;
	ShaderProgram()->bind();
	ShaderProgram()->setUniformValue("lightSpaceMatrix", m_lightSpaceMatrix);
	ShaderProgram()->release();
}