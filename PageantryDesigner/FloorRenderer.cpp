#include "FloorRenderer.h"
#include <QPainter>
#include "GraphicsPanel.h"


FloorRenderer::FloorRenderer(GraphicsPanel* parent) : ItemRenderer(parent)
{
	initialize();
}

FloorRenderer::~FloorRenderer()
{
	delete m_texture;
	delete m_floorProgram;
	delete m_gridProgram;
}

void FloorRenderer::initialize()
{
	initializeOpenGLFunctions();
	initShaders();
	initTextures("../tattoo_comic.jpg");
	setupAttributes();
	generateBuffers();
}

void FloorRenderer::initShaders()
{
	m_floorProgram = new QOpenGLShaderProgram;
	if (!m_floorProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, "floor_vertex.glsl"))
		qDebug() << m_floorProgram->log();
	if (!m_floorProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, "floor_fragment.glsl"))
		qDebug() << m_floorProgram->log();
	if (!m_floorProgram->link())
		qDebug() << m_floorProgram->log();

	m_gridProgram = new QOpenGLShaderProgram;
	if(!m_gridProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, "grid_vertex.glsl"))
		qDebug() << m_gridProgram->log();
	if(m_gridProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, "grid_fragment.glsl"))
		qDebug() << m_gridProgram->log();
	if (!m_gridProgram->link())
		qDebug() << m_gridProgram->log();

	//i still am not sure when this becomes useful and necessary
	if (!m_vao.isCreated())
		m_vao.create();
}

void FloorRenderer::initTextures(const QString& path)
{
	//OpenGL flips the image so it needs to be mirrored when created
	QImage image(path);
	m_texture = new QOpenGLTexture(image.mirrored());

	// Set nearest filtering mode for texture minification
	m_texture->setMinificationFilter(QOpenGLTexture::Nearest);

	// Set bilinear filtering mode for texture magnification
	m_texture->setMagnificationFilter(QOpenGLTexture::Linear);

	// Wrap texture coordinates by repeating
	m_texture->setWrapMode(QOpenGLTexture::Repeat);
}

void FloorRenderer::setupAttributes()
{
	m_posAttr = m_floorProgram->attributeLocation("posAttr");
	m_texCoordAttr = m_floorProgram->attributeLocation("texCoordAttr");
	m_colAttr = m_floorProgram->attributeLocation("colAttr");
	m_matrixUniform = m_floorProgram->uniformLocation("matrix");

	m_gridposAttr = m_gridProgram->attributeLocation("posAttr");
	m_gridcolAttr = m_gridProgram->attributeLocation("colAttr");
	m_gridmatrixUniform = m_gridProgram->uniformLocation("matrix");
}

void FloorRenderer::setMatrix(QMatrix4x4 matrix)
{
	m_floorProgram->bind();
	m_floorProgram->setUniformValue(m_matrixUniform, matrix);
	m_floorProgram->release();
	m_gridProgram->bind();
	m_gridProgram->setUniformValue(m_gridmatrixUniform, matrix);
	m_gridProgram->release();
}

void FloorRenderer::Draw()
{
	drawBuffers();
	//drawFloor();
	//drawGridLines();
}

void FloorRenderer::drawFloor()
{
	VertexData vertices[] =
	{
		{ QVector3D(1.0f,  1.0f, 0.0f), QVector2D(1.0f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f) }, // top right
		{ QVector3D(1.0f, -1.0f, 0.0f), QVector2D(1.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f) }, // bottom right
		{ QVector3D(-1.0f, -1.0f, 0.0f), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f) }, // bottom left
		{ QVector3D(-1.0f,  1.0f, 0.0f), QVector2D(0.0f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f) }  // top left 
	};

	GLushort indices[] =
	{
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	m_floorProgram->bind();

	if (!m_vbo.isCreated())
		m_vbo.create();
	m_vbo.bind();
	m_vbo.allocate(vertices, sizeof(vertices));

	m_floorProgram->enableAttributeArray(m_posAttr);
	m_floorProgram->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 3, sizeof(VertexData));

	m_texture->bind();
	m_floorProgram->enableAttributeArray(m_texCoordAttr);
	m_floorProgram->setAttributeBuffer(m_texCoordAttr, GL_FLOAT, sizeof(decltype(vertices->position)), 2, sizeof(VertexData));

	m_floorProgram->enableAttributeArray(m_colAttr);
	m_floorProgram->setAttributeBuffer(m_colAttr, GL_FLOAT, sizeof(decltype(vertices->position)) + sizeof(decltype(vertices->texCoord)), 3, sizeof(VertexData));
	m_vbo.release();

	if (!m_ebo.isCreated())
		m_ebo.create();
	m_ebo.allocate(indices, sizeof(indices));
	m_ebo.bind();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	m_floorProgram->release();
}

void FloorRenderer::generateFloor()
{

}

int FloorRenderer::generateGridLines(int lineCount)
{
	//GridMaker::createGrid(lineCount, &m_FloorData.gridVertices, m_FloorData.vSize, m_FloorData.gridIndices, m_FloorData.iSize);
	return 1;
}
void FloorRenderer::drawGridLines()
{
	VertexData* vertices;
	GLushort* indices;
	int vSize, iSize;

	GridMaker::createGrid(4, &vertices, vSize, &indices, iSize);

	m_gridProgram->bind();

	if (!m_vbo.isCreated())
		m_vbo.create();
	m_vbo.bind();

	m_vbo.allocate(vertices, vSize * sizeof(VertexData));
	m_gridProgram->enableAttributeArray(m_posAttr);
	m_gridProgram->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 3, sizeof(VertexData));

	m_vbo.release();

	if (!m_ebo.isCreated())
		m_ebo.create();
	m_ebo.allocate(indices, iSize * sizeof(GLushort));
	m_ebo.bind();

	glLineWidth(5.0);
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, 0);
	glLineWidth(2.0);
	glDrawElements(GL_LINES, iSize, GL_UNSIGNED_SHORT, 0);

	m_gridProgram->release();

	delete[] vertices;
	delete[] indices;
}

void FloorRenderer::generateBuffers()
{
	GLfloat vertices[] = 
	{
		1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f
	};

	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	m_floorProgram->bind();
	m_vao.bind();

	if (!m_vbo.isCreated())
		m_vbo.create();
	m_vbo.bind();

	m_vbo.allocate((18 + 18) * sizeof(GLfloat));
	m_vbo.write(0, vertices, sizeof(vertices));
	m_vbo.write(sizeof(vertices), colors, sizeof(colors));

	int pos = m_floorProgram->attributeLocation("posAttr");
	m_floorProgram->enableAttributeArray(m_posAttr);
	m_floorProgram->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 3);

	int color = m_floorProgram->attributeLocation("colAttr");
	m_floorProgram->enableAttributeArray(m_colAttr);
	m_floorProgram->setAttributeBuffer(m_colAttr, GL_FLOAT, sizeof(vertices), 3);

	m_vao.release();
	m_vbo.release();
	m_floorProgram->release();
}

void FloorRenderer::drawBuffers()
{
	m_floorProgram->bind();
	m_vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	m_vao.release();
	m_floorProgram->release();
}