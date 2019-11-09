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
	ItemRenderer::initialize();
	initShaders();
	initTextures("../eyes_base.png");
	m_gridLineCt = generateGridLines(4);
	generateFloor();
}

void FloorRenderer::initShaders()
{
	Floor.initShaders("floor_vertex.glsl", "floor_fragment.glsl");
	Grid.initShaders("grid_vertex.glsl", "grid_fragment.glsl");
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
	std::vector<VertexData> vData;
	std::vector<GLushort> iData;
	GridMaker::Quad::createQuad(vData, iData);

	Floor.bindAll();
	Floor.Vbo().allocate(&vData[0], vData.size() * sizeof(VertexData));

	Floor.ShaderProgram()->enableAttributeArray(Floor.PosAttr());
	Floor.ShaderProgram()->setAttributeBuffer(Floor.PosAttr(), GL_FLOAT, 0, 3, sizeof(VertexData));

	Floor.ShaderProgram()->enableAttributeArray(Floor.TextureAttr());
	Floor.ShaderProgram()->setAttributeBuffer(Floor.TextureAttr(), GL_FLOAT, sizeof(decltype(vData[0].position)), 3, sizeof(VertexData));

	Floor.ShaderProgram()->enableAttributeArray(Floor.ColorAttr());
	Floor.ShaderProgram()->setAttributeBuffer(Floor.ColorAttr(), GL_FLOAT, 
		sizeof(decltype(vData[0].position)) + sizeof(decltype(vData[0].texCoord)) + sizeof(decltype(vData[0].normal)), 4, sizeof(VertexData));

	Floor.Ebo().allocate(&iData[0], iData.size() * sizeof(GLushort));
	Floor.releaseAll();
}

int FloorRenderer::generateGridLines(int lineCount)
{
	std::vector<VertexData> vData;
	std::vector<GLushort> iData;
	GridMaker::Lines::makeGridLines(4, vData, iData);

	Grid.bindAll();
	Grid.Vbo().allocate(&vData[0], vData.size() * sizeof(VertexData));

	Grid.ShaderProgram()->enableAttributeArray(Grid.PosAttr());
	Grid.ShaderProgram()->setAttributeBuffer(Grid.PosAttr(), GL_FLOAT, 0, 3, sizeof(VertexData));

	Grid.Ebo().allocate(&iData[0], iData.size() * sizeof(GLushort));
	Grid.releaseAll();

	return iData.size();
}

void FloorRenderer::drawGridLines()
{
	Grid.bindToDraw();
	glLineWidth(5.0);
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, 0);
	glLineWidth(2.0);
	glDrawElements(GL_LINES, m_gridLineCt, GL_UNSIGNED_SHORT, 0);
	Grid.releaseFromDraw();
}