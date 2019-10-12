#pragma once
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

struct ShaderAttributes
{
	GLuint m_posAttr = -1;
	GLuint m_colAttr = -1;
	GLuint m_texCoordAttr = -1;
	GLuint m_modelUniform = -1;
	GLuint m_viewUniform = -1;
	GLuint m_projectionUniform = -1;
};

class GraphicsObject
{
public:
	GraphicsObject();
	~GraphicsObject();
	QOpenGLShaderProgram* ShaderProgram() const { return m_program.get(); }
	QOpenGLTexture* Texture() const { return m_texture.get(); }
	QOpenGLVertexArrayObject& Vao();
	QOpenGLBuffer Vbo();
	QOpenGLBuffer Ebo();
	GLuint PosAttr() { return m_shaderAttributes.m_posAttr; }
	GLuint ColorAttr() { return m_shaderAttributes.m_colAttr; }
	GLuint TextureAttr() { return m_shaderAttributes.m_texCoordAttr; }
	GLuint ViewAttr() { return m_shaderAttributes.m_viewUniform; }
	GLuint ProjAttr() { return m_shaderAttributes.m_projectionUniform; }
	GLuint ModelAttr() { return m_shaderAttributes.m_modelUniform; }
	void initShader(const QString& vertexPath, const QString& fragmentPath);
	void initTexture(const QString& path);
	void setupAttributes();
	void setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection);
	void bindAll();
	void releaseAll();
	void bindToDraw(bool useTexture = true);
	void releaseFromDraw();

private:
	std::unique_ptr<QOpenGLShaderProgram> m_program;
	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo, m_ebo;
	std::unique_ptr<QOpenGLTexture> m_texture;
	ShaderAttributes m_shaderAttributes;
};

