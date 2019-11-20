#pragma once
#include "common.h"
#include "utils.h"
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLExtraFunctions>

typedef std::vector<GLushort> IndexPool;

static const unsigned int HAS_DIRECTIONAL_LIGHTS = 0x01;
static const unsigned int HAS_POINT_LIGHTS = 0x02;
static const unsigned int HAS_SPOTLIGHTS = 0x04;
static const unsigned int USES_LIGHTS = 0x08;
static const unsigned int USES_MATERIAL_TEXTURES = 0x10;
static const unsigned int HAS_COLOR_DATA = 0x20;

static const char* SCENE_DATA = "sceneData";

namespace Shader
{
	enum Attributes
	{
		Position = 0,
		Color,
		TexCoords,
		Normal,
		Bone0,
		Bone1,
		Bone2,
		Bone3,
		Model,
		View,
		Projection,
		SceneData
	};
};

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
	QOpenGLTexture* SpecularTexture() const { return m_specularTexture.get(); }
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
	void initBuffers() { initBuffers(m_vertexData, m_indices); }
	void initBuffers(VertexDataPool& data, IndexPool& indices);
	void setupBuffers() { setupBuffers(m_vertexData, m_indices); }
	void setupBuffers(VertexDataPool& data, IndexPool& indices);
	void initShaders(const QString& vertexPath, const QString& fragmentPath, bool setup = true);
	void initTexture(const QString& path);
	void initSpecularTexture(const QString& path);
	void setupAttributes();
	void setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection);
	QMatrix4x4& getModelMatrix() { return m_modelMatrix; }
	void setSceneData(unsigned int data) { m_sceneDataMask = data; setSceneDataUniform(); }
	void setSceneDataUniform();
	unsigned int getSceneData() { return m_sceneDataMask; }
	virtual void Draw();
	void bindAll();
	void releaseAll();
	void bindToDraw();
	void releaseFromDraw();
	std::vector<VertexData>& getVertexData() { return m_vertexData; }
	void setVertexData(std::vector<VertexData>& vdata) { m_vertexData.clear();  m_vertexData.assign(vdata.begin(), vdata.end()); }
	std::vector<GLushort>& getIndices() { return m_indices; }
	void setIndices(std::vector<GLushort>& idata) { m_indices.clear(); m_indices.assign(idata.begin(), idata.end()); }
	void setMeshColor(QVector4D& color) { m_meshColor = gammaCorrected(color); }
	QVector4D& getMeshColor() { return m_meshColor; }
	void applyMeshColor();
	void setMaterialData(MaterialData& mdata) { m_materialData = mdata; }
	MaterialData& getMaterialData() { return m_materialData; }
	bool isUsingMaterialData() { return m_usingMaterialData; }
	void setUsingMaterialData(bool usingMd) { m_usingMaterialData = usingMd; }

protected:
	std::vector<VertexData> m_vertexData;
	std::vector<GLushort> m_indices;
	QVector4D m_meshColor;

private:
	std::unique_ptr<QOpenGLShaderProgram> m_program;
	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo, m_ebo;
	MaterialData m_materialData;
	bool m_usingMaterialData = false;
	std::unique_ptr<QOpenGLTexture> m_texture;
	std::unique_ptr<QOpenGLTexture> m_specularTexture;
	ShaderAttributes m_shaderAttributes;
	QMatrix4x4 m_modelMatrix;
	unsigned int m_sceneDataMask = HAS_COLOR_DATA; //defaults to color data because that is set to a guaranteed default value that will actually render...MAYBE...should test
};

