#pragma once
#include "RendererBase.h"
#include "MeshManager.h"
#include "PrimitiveObject.h"

class MeshRenderer : public RendererBase
{
public:
	MeshRenderer(GraphicsPanel* parent);
	MeshRenderer(GraphicsPanel* parent, const QString& importPath);
	~MeshRenderer();

	void importModel(const QString& importPath, bool reset = false);
	void loadTexture(const QString& texturePath, const QString& meshName);
	virtual void Draw() override;
	virtual void setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection) override;
	MeshManager* getMeshManager() { return m_meshManager.get(); }
	void initTextures(const QString& path);

protected:
	virtual void initialize() override;

private:
	//this isn't currently used and is repeated because MeshObject does the same thing
	void initShaders(const QString& vertexPath, const QString& fragmentPath);

	std::unique_ptr<MeshManager> m_meshManager;
	std::unique_ptr<PrimitiveObject> m_primObj;
};

