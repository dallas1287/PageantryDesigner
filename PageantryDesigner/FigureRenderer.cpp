#include "FigureRenderer.h"

FigureRenderer::FigureRenderer(GraphicsPanel* parent, const QString& importPath) : ItemRenderer(parent), m_meshManager(MeshManager(this))
{
	initialize();
	m_meshManager.import(importPath);
	for (auto mesh : m_meshManager.getMeshes())
		mesh->initialize();
}

FigureRenderer::~FigureRenderer()
{
}

void FigureRenderer::initialize()
{
	ItemRenderer::initialize();
}

void FigureRenderer::initShaders(const QString& vertexPath, const QString& fragmentPath)
{
	for(auto mesh : m_meshManager.getMeshes())
		mesh->initShaders("mesh_vertex.glsl", "mesh_fragment.glsl");
}

void FigureRenderer::initTextures(const QString& path)
{
}

void FigureRenderer::setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection)
{
	for (auto mesh : m_meshManager.getMeshes())
		mesh->setMVP(model, view, projection);
}

void FigureRenderer::Draw()
{
	for (auto mesh : m_meshManager.getMeshes())
	{
		mesh->bindToDraw(false);
		glDrawElements(GL_TRIANGLES, mesh->getIndices().size(), GL_UNSIGNED_SHORT, 0);
		mesh->releaseFromDraw();
	}
}