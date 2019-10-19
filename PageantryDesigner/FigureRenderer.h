#pragma once
#include "ItemRenderer.h"
#include "Mesh.h"
class FigureRenderer : public ItemRenderer
{
public:
	FigureRenderer(GraphicsPanel* parent, const QString& importPath);
	~FigureRenderer();

	virtual void Draw() override;
	virtual void setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection) override;
	MeshManager& getMeshManager() { return m_meshManager; }

protected:
	virtual void initialize() override;

private:
	void initShaders(const QString& vertexPath, const QString& fragmentPath);
	void initTextures(const QString& path);

	MeshManager m_meshManager;

};

