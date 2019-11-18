#pragma once
#include <QOpenGLFrameBufferObject>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLExtraFunctions>
#include <QMatrix4x4>
#include "GraphicsObject.h"

static const int SHADOW_MAP_WIDTH = 1024;
static const int SHADOW_MAP_HEIGHT = 1024;

static const char* SHADOW_MAP_VS = "shadowMap_vs.glsl";
static const char* SHADOW_MAP_FRAG = "shadowMap_frag.glsl";

class ShadowMap : public GraphicsObject
{
public:
	ShadowMap();
	virtual ~ShadowMap();
	QOpenGLTexture* DepthMap() const { return m_depthMapTexture.get(); }
	void initDepthMap();
	void initFrameBuffer();
	void setViewport();
	void setLightSpaceMatrix(const QVector3D& lightPos);
	const QMatrix4x4& getLightSpaceMatrix() { return m_lightSpaceMatrix; }
	void setShaderLightSpaceMatrix();
	void setModelUniform(const QMatrix4x4& model);
	void saveBufferAsImage();
	GraphicsObject* getQuad() { return m_quad.get(); }

private:
	std::unique_ptr<QOpenGLFramebufferObject> m_fbo;
	float m_nearPlane = 0.1;
	float m_farPlane = 10.0;
	QMatrix4x4 m_projection;
	QMatrix4x4 m_view;
	QMatrix4x4 m_lightSpaceMatrix;

	std::unique_ptr<QOpenGLTexture> m_depthMapTexture;
	std::unique_ptr<GraphicsObject> m_quad;

};

