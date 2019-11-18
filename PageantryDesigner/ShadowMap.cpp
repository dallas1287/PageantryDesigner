#include "ShadowMap.h"
#include "utils.h"

ShadowMap::ShadowMap() : GraphicsObject()
{
	m_quad.reset(new GraphicsObject());
	VertexDataPool vdata;
	IndexPool idata;
	ShapeMaker::Quad::createQuad(vdata, idata);
	m_quad->setVertexData(vdata);
	m_quad->setIndices(idata);
	m_quad->initShaders("depthMap_vs.glsl", "depthMap_frag.glsl");
	//m_quad->initShaders("mesh_vertex.glsl", "texture_frag.glsl");
	m_quad->initBuffers();
}

ShadowMap::~ShadowMap()
{
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
	if (!ShaderProgram())
		return;
	ShaderProgram()->bind();
	ShaderProgram()->setUniformValue("lightSpaceMatrix", m_lightSpaceMatrix);
	ShaderProgram()->release();
}

void ShadowMap::setModelUniform(const QMatrix4x4& model)
{
	if (!ShaderProgram())
		return;
	ShaderProgram()->bind();
	ShaderProgram()->setUniformValue("model", m_lightSpaceMatrix);
	ShaderProgram()->release();
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