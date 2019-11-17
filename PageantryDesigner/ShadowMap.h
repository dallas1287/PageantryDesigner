#pragma once
#include <QOpenGLFrameBufferObject>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLExtraFunctions>

static const int SHADOW_MAP_WIDTH = 1024;
static const int SHADOW_MAP_HEIGHT = 1024;

static const char* SHADOW_MAP_VS = "shadowMap_vs.glsl";
static const char* SHADOW_MAP_FRAG = "shadowMap_frag.glsl";

class ShadowMap : protected QOpenGLExtraFunctions
{
public:
	ShadowMap();
	virtual ~ShadowMap();
	QOpenGLShaderProgram* ShaderProgram() const { return m_program.get(); }
	QOpenGLTexture* DepthMap() const { return m_depthMapTexture.get(); }
	QOpenGLVertexArrayObject& Vao();
	QOpenGLBuffer Vbo();
	GLuint PosAttr() { return m_posAttr; }
	void initShader();
	void initDepthMap();
	void initFrameBuffer();
	void bindToDraw();
	void setViewport();

private:
	std::unique_ptr<QOpenGLShaderProgram> m_program;
	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo, m_ebo;
	std::unique_ptr<QOpenGLFramebufferObject> m_fbo;
	GLuint m_posAttr = -1;

	std::unique_ptr<QOpenGLTexture> m_depthMapTexture;

};

