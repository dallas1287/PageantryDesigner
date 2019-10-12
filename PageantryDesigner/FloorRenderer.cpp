#include "FloorRenderer.h"
#include "ShapeCreator.h"

FloorRenderer::FloorRenderer(GraphicsPanel* parent) : ItemRenderer(parent)
{
	initialize();
}

FloorRenderer::~FloorRenderer()
{
}

void FloorRenderer::initialize()
{
	initializeOpenGLFunctions();
	initShaders();
	initTextures("../tattoo_comic.jpg");
	m_gridLineCt = generateGridLines(4);
	generateFloor();
}

void FloorRenderer::initShaders()
{
	Floor.initShader("floor_vertex.glsl", "floor_fragment.glsl");
	Grid.initShader("grid_vertex.glsl", "grid_fragment.glsl");
}

void FloorRenderer::initTextures(const QString& path)
{
	Floor.initTexture(path);
}

void FloorRenderer::setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection)
{
	Floor.setMVP(model, view, projection);
	Grid.setMVP(model, view, projection);
}

void FloorRenderer::Draw()
{
	drawFloor();
	drawGridLines();
}

void FloorRenderer::drawFloor()
{
	Floor.bindToDraw();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	Floor.releaseFromDraw();
}

void FloorRenderer::generateFloor()
{
	VertexData* vertices;
	GLushort* indices;
	int vSize;
	int iSize;
	GridMaker::Quad::createQuad(&vertices, vSize, &indices, iSize);

	Floor.bindAll();
	Floor.Vbo().allocate(vertices, vSize * sizeof(VertexData));

	Floor.ShaderProgram()->enableAttributeArray(Floor.PosAttr());
	Floor.ShaderProgram()->setAttributeBuffer(Floor.PosAttr(), GL_FLOAT, 0, 3, sizeof(VertexData));

	Floor.ShaderProgram()->enableAttributeArray(Floor.TextureAttr());
	Floor.ShaderProgram()->setAttributeBuffer(Floor.TextureAttr(), GL_FLOAT, sizeof(decltype(vertices->position)), 2, sizeof(VertexData));

	Floor.ShaderProgram()->enableAttributeArray(Floor.ColorAttr());
	Floor.ShaderProgram()->setAttributeBuffer(Floor.ColorAttr(), GL_FLOAT, sizeof(decltype(vertices->position)) + sizeof(decltype(vertices->texCoord)), 3, sizeof(VertexData));

	Floor.Ebo().allocate(indices, iSize * sizeof(GLushort));
	Floor.releaseAll();

	delete[] vertices;
	delete[] indices;
}

int FloorRenderer::generateGridLines(int lineCount)
{
	VertexData* vertices;
	GLushort* indices;
	int vSize, iSize;
	GridMaker::Lines::createGrid(4, &vertices, vSize, &indices, iSize);

	Grid.bindAll();
	Grid.Vbo().allocate(vertices, vSize * sizeof(VertexData));
	
	Grid.ShaderProgram()->enableAttributeArray(Grid.PosAttr());
	Grid.ShaderProgram()->setAttributeBuffer(Grid.PosAttr(), GL_FLOAT, 0, 3, sizeof(VertexData));

	Grid.Ebo().allocate(indices, iSize * sizeof(GLushort));
	Grid.releaseAll();
	
	delete[] vertices;
	delete[] indices;

	return iSize;
}
void FloorRenderer::drawGridLines()
{
	Grid.bindToDraw(false);
	glLineWidth(5.0);
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, 0);
	glLineWidth(2.0);
	glDrawElements(GL_LINES, m_gridLineCt, GL_UNSIGNED_SHORT, 0);
	Grid.releaseFromDraw();
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
	Floor.ShaderProgram()->bind();
	Floor.Vao().bind();
	Floor.Vbo().bind();

	Floor.Vbo().allocate((18 + 18) * sizeof(GLfloat));
	Floor.Vbo().write(0, vertices, sizeof(vertices));
	Floor.Vbo().write(sizeof(vertices), colors, sizeof(colors));

	Floor.ShaderProgram()->enableAttributeArray(Floor.PosAttr());
	Floor.ShaderProgram()->setAttributeBuffer(Floor.PosAttr(), GL_FLOAT, 0, 3);

	Floor.ShaderProgram()->enableAttributeArray(Floor.ColorAttr());
	Floor.ShaderProgram()->setAttributeBuffer(Floor.ColorAttr(), GL_FLOAT, sizeof(vertices), 3);

	Floor.Vao().release();
	Floor.Vbo().release();
	Floor.ShaderProgram()->release();
}

void FloorRenderer::drawBuffers()
{
	Floor.ShaderProgram()->bind();
	Floor.Vao().bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	Floor.Vao().release();
	Floor.ShaderProgram()->release();
}