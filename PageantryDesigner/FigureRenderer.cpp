#include "FigureRenderer.h"
#include <QMessageBox>
#include "GraphicsPanel.h"

FigureRenderer::FigureRenderer(GraphicsPanel* parent) : ItemRenderer(parent)
{
	m_meshManager.reset(new MeshManager(this));
}

FigureRenderer::FigureRenderer(GraphicsPanel* parent, const QString& importPath) : ItemRenderer(parent)
{
	m_meshManager.reset(new MeshManager(this));
	importModel(importPath);
}

FigureRenderer::~FigureRenderer()
{
}

void FigureRenderer::importModel(const QString& importPath, bool reset)
{
	if(reset)
		m_meshManager->resetData();

	if (importPath.isEmpty() || !m_meshManager->import(importPath))
	{
		QMessageBox msgBox;
		QMessageBox::warning(m_parent->getParent(), "Loading Error",  importPath.isEmpty() ? "No file path given." : "Unable to load file");
		return;
	}
	for (auto mesh : m_meshManager->getMeshes())
		mesh->initialize();
}

void FigureRenderer::loadTexture(const QString& texturePath, const QString& meshName)
{
	MeshObject* meshObj = m_meshManager->findMesh(meshName);

	if (meshObj && !texturePath.isEmpty())
	{
		meshObj->initTexture(texturePath);
		meshObj->initShaders("mesh_vertex.glsl", "texture_frag.glsl");
	}
}

void FigureRenderer::initialize()
{
	ItemRenderer::initialize();
}

//generic shader initialization. this most likely will get re-initialized per mesh 
void FigureRenderer::initShaders(const QString& vertexPath, const QString& fragmentPath)
{
	for(auto mesh : m_meshManager->getMeshes())
		mesh->initShaders("mesh_vertex.glsl", "mesh_fragment.glsl");
}

void FigureRenderer::initTextures(const QString& path)
{
	if (path.isEmpty())
		return;

	for (auto mesh : getMeshManager()->getMeshes())
		mesh->initTexture(path);
}

void FigureRenderer::setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection)
{
	for (auto mesh : m_meshManager->getMeshes())
		mesh->setMVP(model, view, projection);
}

void FigureRenderer::Draw()
{
	for (auto mesh : m_meshManager->getMeshes())
	{
		//MeshObject* mesh = m_meshManager->getMeshes()[0];
		mesh->bindToDraw();
		glDrawElements(GL_TRIANGLES, mesh->getIndices().size(), GL_UNSIGNED_SHORT, 0);
		mesh->releaseFromDraw();
	}
}