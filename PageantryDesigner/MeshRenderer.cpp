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

void MeshRenderer::initShaders(const QString& vertexPath, const QString& fragmentPath)
{
	m_program.reset(new QOpenGLShaderProgram);
	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexPath))
		qDebug() << m_program->log();
	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentPath))
		qDebug() << m_program->log();
	if (!m_program->link())
	{
		qDebug() << m_program->log();
		return;
	}

	m_program->bindAttributeLocation("posAttr", Shader::Position);
	m_program->bindAttributeLocation("texCoordAttr", Shader::TexCoords);
	m_program->bindAttributeLocation("normAttr", Shader::Normal);
	m_program->bindAttributeLocation("colAttr", Shader::Color);
	m_program->bindAttributeLocation("boneTransform0", Shader::Bone0);
	m_program->bindAttributeLocation("boneTransform1", Shader::Bone1);
	m_program->bindAttributeLocation("boneTransform2", Shader::Bone2);
	m_program->bindAttributeLocation("boneTransform3", Shader::Bone3);
}

void MeshRenderer::initTextures(const QString& path)
{
	if (path.isEmpty())
		return;

	for (auto mesh : getMeshManager()->getMeshes())
		mesh->initTexture(path);
}

void MeshRenderer::initFrameBuffer()
{
	getShadowMap()->m_fbo.reset(new QOpenGLFramebufferObject(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, QOpenGLFramebufferObject::Attachment::Depth));

	if (!getShadowMap()->DepthMap() || !getShadowMap()->DepthMap()->isCreated())
		getShadowMap()->initDepthMap();

	getShadowMap()->Fbo()->bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, getShadowMap()->DepthMap()->textureId(), 0);
	//this differs from the example in LearOpenGL in that it calls glDrawBuffer(GL_NONE) which doesn't seem to be available in the qt OpenGLExtraFunctions 
	GLenum buf = GL_NONE;
	glDrawBuffers(1, &buf);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		qDebug() << "OpenGL Frambuffer status not complete.";

	getShadowMap()->Fbo()->release();
}

void MeshRenderer::writeFrameBuffer()
{
	QVector3D lightPos(-2.0f, 4.0f, -1.0);

	getShadowMap()->setLightSpaceMatrix(lightPos);

	getShadowMap()->setViewport();
	getShadowMap()->Fbo()->bind();

	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	QMatrix4x4 model;
	int ct = 0;
	for (auto pObj : PrimitiveObjects())
	{
		if (pObj->getType() == Primitive::Quad)
		{			
			model.setToIdentity();
			model.translate(0.0, -0.5, 0.0);
			model.rotate(-90.0, X);
			getShadowMap()->setModelUniform(model);
			Draw(pObj, DrawType::Shadow);
		}
		else
		{
			model.setToIdentity();
			if (ct == 0)
			{
				model.translate(0.0, 1.5, 0.0);
				model.scale(0.5);
			}
			else if (ct == 1)
			{
				model.translate(2.0, 0.0, 1.0);
				model.scale(0.5);
			}
			else if (ct == 2)
			{
				model.translate(-1.0, 0.0, 2.0);
				model.rotate(60.0, QVector3D(1.0, 0.0, 1.0));
				model.scale(.25);
			}
			getShadowMap()->setModelUniform(model);
			Draw(pObj, DrawType::Shadow);
			ct++;
		}
	}
	getShadowMap()->Fbo()->release();
}

void MeshRenderer::setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection)
{
	m_program->bind();
	m_program->setUniformValue(m_program->uniformLocation("model"), model);
	m_program->setUniformValue(m_program->uniformLocation("view"), view);
	m_program->setUniformValue(m_program->uniformLocation("projection"), projection);
	m_program->release();
}

void MeshRenderer::Draw()
{
	for (auto mesh : m_meshManager->getMeshes())
		mesh->Draw();

	for (auto pObj : m_primitiveObjects)
		pObj->Draw();
}

void MeshRenderer::Draw(GraphicsObject* obj, int type)
{
	QOpenGLShaderProgram* shaderProgram = nullptr;
	if (type == DrawType::Screen)
		shaderProgram = ShaderProgram();
	else if (type == DrawType::Shadow)
		shaderProgram = getShadowMap()->ShaderProgram();
	else
		return;

	shaderProgram->bind();
	obj->Vao().bind();
	glDrawElements(GL_TRIANGLES, obj->getIndices().size(), GL_UNSIGNED_SHORT, 0);
	obj->Vao().release();
	shaderProgram->release();
}

void MeshRenderer::DrawAll()
{
	for (GraphicsObject* obj : PrimitiveObjects())
		Draw(obj);
}

void MeshRenderer::renderShadowDepthMap()
{
	getShadowMap()->getQuad()->ShaderProgram()->bind();
	getShadowMap()->getQuad()->ShaderProgram()->setUniformValue("near_plane", getShadowMap()->getNearPlane());
	getShadowMap()->getQuad()->ShaderProgram()->setUniformValue("far_plane", getShadowMap()->getFarPlane());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, getShadowMap()->DepthMap()->textureId());
	getShadowMap()->getQuad()->Draw();
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