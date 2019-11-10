#include "MeshRenderer.h"

#include "MeshRenderer.h"
#include <QMessageBox>
#include "GraphicsPanel.h"

MeshRenderer::MeshRenderer(GraphicsPanel* parent) : RendererBase(parent)
{
	m_meshManager.reset(new MeshManager(this));
}

MeshRenderer::MeshRenderer(GraphicsPanel* parent, const QString& importPath) : RendererBase(parent)
{
	m_meshManager.reset(new MeshManager(this));
	importModel(importPath);
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::importModel(const QString& importPath, bool reset)
{
	if (reset)
		m_meshManager->resetData();

	if (importPath.isEmpty() || !m_meshManager->import(importPath))
	{
		QMessageBox msgBox;
		QMessageBox::warning(m_parent->getParent(), "Loading Error", importPath.isEmpty() ? "No file path given." : "Unable to load file");
		return;
	}
	for (auto mesh : m_meshManager->getMeshes())
		mesh->initialize();
}

void MeshRenderer::loadTexture(const QString& texturePath, const QString& meshName)
{
	MeshObject* meshObj = m_meshManager->findMesh(meshName);

	if (meshObj && !texturePath.isEmpty())
	{
		meshObj->initTexture(texturePath);
		meshObj->initShaders("mesh_vertex.glsl", "texture_frag.glsl");
	}
}

void MeshRenderer::initialize()
{
	RendererBase::initialize();
}

//generic shader initialization. this most likely will get re-initialized per mesh 
void MeshRenderer::initShaders(const QString& vertexPath, const QString& fragmentPath)
{
	for (auto mesh : m_meshManager->getMeshes())
		mesh->initShaders("mesh_vertex.glsl", "mesh_fragment.glsl");
}

void MeshRenderer::initTextures(const QString& path)
{
	if (path.isEmpty())
		return;

	for (auto mesh : getMeshManager()->getMeshes())
		mesh->initTexture(path);
}

void MeshRenderer::setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection)
{
	for (auto mesh : m_meshManager->getMeshes())
		mesh->setMVP(model, view, projection);
}

void MeshRenderer::Draw()
{
	for (auto mesh : m_meshManager->getMeshes())
	{
		mesh->bindToDraw();
		glDrawElements(GL_TRIANGLES, mesh->getIndices().size(), GL_UNSIGNED_SHORT, 0);
		mesh->releaseFromDraw();
	}
}