#include "GraphicsObject.h"

GraphicsObject::GraphicsObject() : m_ebo(QOpenGLBuffer(QOpenGLBuffer::IndexBuffer))
{
	initializeOpenGLFunctions();
	//initialize arrays and attributes
	Vao();
	Vbo();
	Ebo();
}

GraphicsObject::~GraphicsObject()
{
}

QOpenGLVertexArrayObject& GraphicsObject::Vao()
{
	if (!m_vao.isCreated())
		m_vao.create();
	return m_vao;
}

QOpenGLBuffer GraphicsObject::Vbo()
{
	if (!m_vbo.isCreated())
		m_vbo.create();
	return m_vbo;
}

QOpenGLBuffer GraphicsObject::Ebo()
{
	if (!m_ebo.isCreated())
		m_ebo.create();
	return m_ebo;
}

void GraphicsObject::initShaders(const QString& vertexPath, const QString& fragmentPath, bool setup)
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
	if(setup)
		setupAttributes();
}

void GraphicsObject::initTexture(const QString& path)
{
	//OpenGL flips the image so it needs to be mirrored when created
	m_texture.reset(new QOpenGLTexture(QImage(path).mirrored()));

	// Set nearest filtering mode for texture minification
	m_texture->setMinificationFilter(QOpenGLTexture::Nearest);

	// Set bilinear filtering mode for texture magnification
	m_texture->setMagnificationFilter(QOpenGLTexture::Linear);

	// Wrap texture coordinates by repeating
	m_texture->setWrapMode(QOpenGLTexture::Repeat);
}

void GraphicsObject::initSpecularTexture(const QString& path)
{
	//OpenGL flips the image so it needs to be mirrored when created
	m_specularTexture.reset(new QOpenGLTexture(QImage(path).mirrored()));

	// Set nearest filtering mode for texture minification
	m_specularTexture->setMinificationFilter(QOpenGLTexture::Nearest);

	// Set bilinear filtering mode for texture magnification
	m_specularTexture->setMagnificationFilter(QOpenGLTexture::Linear);

	// Wrap texture coordinates by repeating
	m_specularTexture->setWrapMode(QOpenGLTexture::Repeat);
}

void GraphicsObject::setupAttributes()
{
	m_shaderAttributes.m_posAttr = m_program->attributeLocation("posAttr");
	m_shaderAttributes.m_texCoordAttr = m_program->attributeLocation("texCoordAttr");
	m_shaderAttributes.m_colAttr = m_program->attributeLocation("colAttr");
	m_shaderAttributes.m_normalAttr = m_program->attributeLocation("normAttr");
	m_shaderAttributes.m_boneTransform0 = m_program->attributeLocation("boneTransform0");
	m_shaderAttributes.m_boneTransform1 = m_program->attributeLocation("boneTransform1");
	m_shaderAttributes.m_boneTransform2 = m_program->attributeLocation("boneTransform2");
	m_shaderAttributes.m_boneTransform3 = m_program->attributeLocation("boneTransform3");
	m_shaderAttributes.m_modelUniform = m_program->uniformLocation("model");
	m_shaderAttributes.m_viewUniform = m_program->uniformLocation("view");
	m_shaderAttributes.m_projectionUniform = m_program->uniformLocation("projection");
}

void GraphicsObject::setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection)
{
	m_program->bind();
	m_modelMatrix = model;
	m_program->setUniformValue(ModelAttr(), model);
	m_program->setUniformValue(ViewAttr(), view);
	m_program->setUniformValue(ProjAttr(), projection);
	m_program->release();
}

void GraphicsObject::setSceneDataUniform()
{
	m_program->bind();
	m_program->setUniformValue(SCENE_DATA, m_sceneDataMask);
	m_program->release();
}

void GraphicsObject::bindAll()
{
	m_program->bind();
	m_vao.bind();
	m_vbo.bind();
	m_ebo.bind();
}

void GraphicsObject::releaseAll()
{
	m_vao.release();
	m_ebo.release();
	m_vbo.release();
	m_program->release();
}

void GraphicsObject::bindToDraw()
{
	if (!m_program)
		return;
	m_program->bind();

	if (m_texture)
	{
		glActiveTexture(GL_TEXTURE3);
		m_texture->bind(GL_TEXTURE3);
		m_program->setUniformValue("material.diffuseTex", GL_TEXTURE3 - GL_TEXTURE0);
	}
	if (m_specularTexture)
	{
		glActiveTexture(GL_TEXTURE4);
		m_specularTexture->bind(GL_TEXTURE4);
		m_program->setUniformValue("material.specularTex", GL_TEXTURE4 - GL_TEXTURE0);
	}

	m_vao.bind();
}

void GraphicsObject::releaseFromDraw()
{
	m_vao.release();
	if(m_texture && m_texture->isBound())
		m_texture->release();
	if (m_specularTexture && m_specularTexture->isBound())
		m_specularTexture->release();
	if(m_program)
		m_program->release();
}

void GraphicsObject::initBuffers(VertexDataPool& data, IndexPool& indices)
{
	if (data.empty() || indices.empty())
		return;
	
	bindAll();
	Vbo().allocate(&data[0], data.size() * sizeof(VertexData));

	int offset = 0;

	ShaderProgram()->enableAttributeArray(PosAttr());
	ShaderProgram()->setAttributeBuffer(PosAttr(), GL_FLOAT, offset, 3, sizeof(VertexData));

	offset += sizeof(decltype(data[0].position));

	ShaderProgram()->enableAttributeArray(TextureAttr());
	ShaderProgram()->setAttributeBuffer(TextureAttr(), GL_FLOAT, offset, 3, sizeof(VertexData));

	offset += sizeof(decltype(data[0].texCoord));

	ShaderProgram()->enableAttributeArray(NormAttr());
	ShaderProgram()->setAttributeBuffer(NormAttr(), GL_FLOAT, offset, 3, sizeof(VertexData));

	offset += sizeof(decltype(data[0].normal));

	ShaderProgram()->enableAttributeArray(ColorAttr());
	ShaderProgram()->setAttributeBuffer(ColorAttr(), GL_FLOAT, offset, 4, sizeof(VertexData));

	offset += sizeof(decltype(data[0].color));

	ShaderProgram()->enableAttributeArray(BoneAttr0());
	ShaderProgram()->setAttributeBuffer(BoneAttr0(), GL_FLOAT, offset, 4, sizeof(VertexData));

	offset += sizeof(QVector4D);

	ShaderProgram()->enableAttributeArray(BoneAttr1());
	ShaderProgram()->setAttributeBuffer(BoneAttr1(), GL_FLOAT, offset, 4, sizeof(VertexData));

	offset += sizeof(QVector4D);

	ShaderProgram()->enableAttributeArray(BoneAttr2());
	ShaderProgram()->setAttributeBuffer(BoneAttr2(), GL_FLOAT, offset, 4, sizeof(VertexData));

	offset += sizeof(QVector4D);

	ShaderProgram()->enableAttributeArray(BoneAttr3());
	ShaderProgram()->setAttributeBuffer(BoneAttr3(), GL_FLOAT, offset, 4, sizeof(VertexData));

	Ebo().allocate(&indices[0], indices.size() * sizeof(GLushort));
	releaseAll();
}

void GraphicsObject::Draw()
{
	if (getIndices().empty())
		return;
	bindToDraw();
	glDrawElements(GL_TRIANGLES, getIndices().size(), GL_UNSIGNED_SHORT, 0);
	releaseFromDraw();
}

void GraphicsObject::applyMeshColor()
{
	for (int i = 0; i < m_vertexData.size(); ++i)
		m_vertexData[i].color = m_meshColor;

	m_materialData.diffuse = QVector3D(m_meshColor);

	initBuffers(m_vertexData, m_indices);
}