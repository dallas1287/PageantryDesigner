#include "MeshRenderer.h"

#include "MeshRenderer.h"
#include <QMessageBox>
#include "GraphicsPanel.h"

MeshRenderer::MeshRenderer(GraphicsPanel* parent) : RendererBase(parent)
{
	m_meshManager.reset(new MeshManager(this));
	m_shadowMap.reset(new ShadowMap());
}

MeshRenderer::MeshRenderer(GraphicsPanel* parent, const QString& importPath) : RendererBase(parent)
{
	m_meshManager.reset(new MeshManager(this));
	if(!importPath.isEmpty())
		importModel(importPath);
}

MeshRenderer::~MeshRenderer()
{
	for (auto pObj : m_primitiveObjects)
		delete pObj;
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
		meshObj->initShaders("mesh_vertex.glsl", "light_frag.glsl");
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
		mesh->Draw();

	for (auto pObj : m_primitiveObjects)
		pObj->Draw();
}

void MeshRenderer::DrawToShadowMap(const QVector3D& lightPos)
{
	getShadowMap()->initDepthMap();
	getShadowMap()->initFrameBuffer();
	glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
	getShadowMap()->setLightSpaceMatrix(lightPos);

	for (auto mesh : m_meshManager->getMeshes())
	{
		mesh->ShaderProgram()->setUniformValue("model", mesh->getModelMatrix());
		mesh->ShaderProgram()->setUniformValue("lightSpaceMatrix", getShadowMap()->getLightSpaceMatrix());
		mesh->Draw();
	}
	for (auto pObj : m_primitiveObjects)
	{
		pObj->ShaderProgram()->setUniformValue("model", pObj->getModelMatrix());
		pObj->ShaderProgram()->setUniformValue("lightSpaceMatrix", getShadowMap()->getLightSpaceMatrix());
		pObj->Draw();
	}
	//getShadowMap()->ShaderProgram()->release();
	getShadowMap()->saveBufferAsImage();
}

void MeshRenderer::createCube(int count)
{
	for (int i = 0; i < count; ++i)
		m_primitiveObjects.emplace_back(new PrimitiveObject(Primitive::Type::Cube));
}

void MeshRenderer::createQuad(int count)
{
	for (int i = 0; i < count; ++i)
		m_primitiveObjects.emplace_back(new PrimitiveObject(Primitive::Type::Quad));
}