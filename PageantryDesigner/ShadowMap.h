#pragma once
#include <QOpenGLFrameBufferObject>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLExtraFunctions>
#include <QMatrix4x4>
#include "GraphicsObject.h"

namespace ShadowType
{
	enum
	{
		NoType = -1,
		Directional = 0,
		Point
	};
}

static const int SHADOW_MAP_WIDTH = 2048;
static const int SHADOW_MAP_HEIGHT = 2048;

class ShadowMap : public GraphicsObject
{
public:
	ShadowMap();
	virtual ~ShadowMap();
	QOpenGLTexture* DepthMap() const { return m_depthMapTexture.get(); }
	QOpenGLFramebufferObject* Fbo() { return m_fbo.get(); }
	virtual void initDepthMap() = 0;
	virtual void initFrameBuffer() = 0;
	void setViewport();
	float getNearPlane() { return m_nearPlane; }
	void setNearPlane(float val) { m_nearPlane = val; }
	float getFarPlane() { return m_farPlane; }
	void setFarPlane(float val) { m_farPlane = val; }
	void setModelUniform(const QMatrix4x4& model);
	GraphicsObject* getQuad() { return m_quad.get(); }
	int getType() { return m_type; }

	std::unique_ptr<QOpenGLFramebufferObject> m_fbo;

protected:
	void initDepthQuad();

	float m_nearPlane = 0.0;
	float m_farPlane = 0.0;

	int m_type = ShadowType::NoType;

	std::unique_ptr<QOpenGLTexture> m_depthMapTexture;
	std::unique_ptr<GraphicsObject> m_quad;
};

