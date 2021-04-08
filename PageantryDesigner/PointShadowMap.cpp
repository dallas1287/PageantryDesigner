#include "PointShadowMap.h"

PointShadowMap::PointShadowMap() 
{
	m_type = ShadowType::Point;
	m_nearPlane = 1.0;
	m_farPlane = 25.0;
	initShadowTransforms();
}

PointShadowMap::~PointShadowMap()
{
}

void PointShadowMap::initShadowTransforms(const QVector3D& lightPos)
{
	m_shadowTransforms.clear();
	QMatrix4x4 shadowProj;
	shadowProj.perspective(90.0, (float)SHADOW_MAP_WIDTH / (float)SHADOW_MAP_HEIGHT, m_nearPlane, m_farPlane);
	for (auto transform : SHADOW_TRANSFORM_MAP)
	{
		QMatrix4x4 lookPos;
		lookPos.lookAt(lightPos, lightPos + transform.first, transform.second);
		m_shadowTransforms.push_back(shadowProj * lookPos);
	}
}

void PointShadowMap::initDepthMap()
{
	m_depthMapTexture.reset(new QOpenGLTexture(QOpenGLTexture::Target::TargetCubeMap));
	m_depthMapTexture->setFormat(QOpenGLTexture::DepthFormat);
	m_depthMapTexture->setDepthStencilMode(QOpenGLTexture::DepthMode);
	m_depthMapTexture->setSize(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
	m_depthMapTexture->setMinificationFilter(QOpenGLTexture::Nearest);
	m_depthMapTexture->setMagnificationFilter(QOpenGLTexture::Nearest);
	m_depthMapTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
	m_depthMapTexture->allocateStorage();

	m_depthMapTexture->bind();
}

void PointShadowMap::initFrameBuffer()
{
	m_fbo.reset(new QOpenGLFramebufferObject(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, QOpenGLFramebufferObject::Attachment::Depth, GL_TEXTURE_CUBE_MAP));
	m_fbo->bind();

	if (!m_depthMapTexture || !m_depthMapTexture->isCreated())
		initDepthMap();
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthMapTexture->textureId(), 0);
	GLenum buf = GL_NONE;
	glDrawBuffers(1, &buf);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		qDebug() << "OpenGL Frambuffer status not complete.";

	m_fbo->release();
}

void PointShadowMap::setShaderShadowTransforms()
{
	ShaderProgram()->bind();
	for (int i = 0; i < 6; ++i)
	{
		QString name("shadowMatrices[" + QString::number(i) + "]");
		ShaderProgram()->setUniformValue(name.toLocal8Bit().constData(), m_shadowTransforms[i]);
	}
	ShaderProgram()->release();
}

void PointShadowMap::setShaderFarPlane()
{
	ShaderProgram()->bind();
	ShaderProgram()->setUniformValue("far_plane", m_farPlane);
	ShaderProgram()->release();
}

void PointShadowMap::setShaderLightPosition(const QVector3D& lightPos)
{
	ShaderProgram()->bind();
	ShaderProgram()->setUniformValue("lightPos", lightPos);
	ShaderProgram()->release();
}