#pragma once
#include "common.h"
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLExtraFunctions>

typedef std::vector<GLushort> IndexPool;

struct ShaderAttributes
{
	GLuint m_posAttr = -1;
	GLuint m_colAttr = -1;
	GLuint m_texCoordAttr = -1;
	GLuint m_normalAttr = -1;
	GLuint m_boneTransform0 = -1;
	GLuint m_boneTransform1 = -1;
	GLuint m_boneTransform2 = -1;
	GLuint m_boneTransform3 = -1;
	GLuint m_modelUniform = -1;
	GLuint m_viewUniform = -1;
	GLuint m_projectionUniform = -1;
};

class GraphicsObject : protected QOpenGLExtraFunctions
{
public:
	GraphicsObject();
	virtual ~GraphicsObject();
	QOpenGLShaderProgram* ShaderProgram() const { return m_program.get(); }
	QOpenGLTexture* Texture() const { return m_texture.get(); }
	QOpenGLVertexArrayObject& Vao();
	QOpenGLBuffer Vbo();
	QOpenGLBuffer Ebo();
	GLuint PosAttr() { return m_shaderAttributes.m_posAttr; }
	GLuint ColorAttr() { return m_shaderAttributes.m_colAttr; }
	GLuint TextureAttr() { return m_shaderAttributes.m_texCoordAttr; }
	GLuint NormAttr() { return m_shaderAttributes.m_normalAttr; }
	GLuint ViewAttr() { return m_shaderAttributes.m_viewUniform; }
	GLuint ProjAttr() { return m_shaderAttributes.m_projectionUniform; }
	GLuint ModelAttr() { return m_shaderAttributes.m_modelUniform; }
	GLuint BoneAttr0() { return m_shaderAttributes.m_boneTransform0; }
	GLuint BoneAttr1() { return m_shaderAttributes.m_boneTransform1; }
	GLuint BoneAttr2() { return m_shaderAttributes.m_boneTransform2; }
	GLuint BoneAttr3() { return m_shaderAttributes.m_boneTransform3; }
	void initBuffers(VertexDataPool& data, IndexPool& indices);
	void initShaders(const QString& vertexPath, const QString& fragmentPath);
	void initTexture(const QString& path);
	void setupAttributes();
	void setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection);
	void bindAll();
	void releaseAll();
	void bindToDraw();
	void releaseFromDraw();
	std::vector<VertexData>& getVertexData() { return m_meshData; }
	std::vector<GLushort>& getIndices() { return m_indices; }

protected:
	std::vector<VertexData> m_meshData;
	std::vector<GLushort> m_indices;

private:
	std::unique_ptr<QOpenGLShaderProgram> m_program;
	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo, m_ebo;
	std::unique_ptr<QOpenGLTexture> m_texture;
	ShaderAttributes m_shaderAttributes;
};

