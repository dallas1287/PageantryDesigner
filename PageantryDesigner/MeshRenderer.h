#pragma once
#include "RendererBase.h"
#include "MeshManager.h"
#include "PrimitiveObject.h"
#include "ShadowMap.h"

namespace DrawType
{
	enum
	{
		Screen,
		Shadow
	};
}

class MeshRenderer : public RendererBase
{
public:
	MeshRenderer(GraphicsPanel* parent);
	MeshRenderer(GraphicsPanel* parent, const QString& importPath);
	~MeshRenderer();

	virtual void Draw() override;
	virtual void setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection) override;

	void importModel(const QString& importPath, bool reset = false);
	void loadTexture(const QString& texturePath, const QString& meshName);

	void Draw(GraphicsObject* obj, int type = DrawType::Screen);
	void DrawAll();
	void renderShadowDepthMap();
	MeshManager* getMeshManager() { return m_meshManager.get(); }
	void initTextures(const QString& path);
	ShadowMap* getShadowMap() { return m_shadowMap.get(); }
	QOpenGLShaderProgram* ShaderProgram() { return m_program.get(); }
	void initShaders(const QString& vertexPath, const QString& fragmentPath);
	void initFrameBuffer();
	void writeFrameBuffer();
	std::vector<PrimitiveObject*>& PrimitiveObjects() { return m_primitiveObjects; }
	void createCube(int count = 1);
	void createQuad(int count = 1);

protected:
	virtual void initialize() override;

private:


	std::unique_ptr<MeshManager> m_meshManager;
	std::vector<PrimitiveObject*> m_primitiveObjects;
	std::unique_ptr<ShadowMap> m_shadowMap;
	std::unique_ptr<QOpenGLShaderProgram> m_program;
};

