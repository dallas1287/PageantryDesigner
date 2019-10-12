#include "DotsRenderer.h"
#include <QPainter>
#include "ShapeCreator.h"

DotsRenderer::DotsRenderer(GraphicsPanel* parent) : ItemRenderer(parent)
{
	initialize();
}

DotsRenderer::~DotsRenderer()
{
}

void DotsRenderer::initialize()
{
	initializeOpenGLFunctions();
	initShaders();
	initTextures("../flag.png");
	generateDots();
}

void DotsRenderer::initShaders()
{
	Dots.initShader("dots_vertex.glsl", "dots_fragment.glsl");
}

void DotsRenderer::initTextures(const QString& path)
{
	Dots.initTexture(path);
}

void DotsRenderer::setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection)
{
	Dots.setMVP(model, view, projection);
}

void DotsRenderer::Draw()
{
	Dots.bindToDraw();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	Dots.releaseFromDraw();
}

void DotsRenderer::generateDots()
{
	VertexData* vertices;
	GLushort* indices;
	int vSize;
	int iSize;
	GridMaker::Quad::createQuad(&vertices, vSize, &indices, iSize);

	Dots.bindAll();
	Dots.Vbo().allocate(vertices, vSize * sizeof(VertexData));

	Dots.ShaderProgram()->enableAttributeArray(Dots.PosAttr());
	Dots.ShaderProgram()->setAttributeBuffer(Dots.PosAttr(), GL_FLOAT, 0, 3, sizeof(VertexData));

	Dots.ShaderProgram()->enableAttributeArray(Dots.TextureAttr());
	Dots.ShaderProgram()->setAttributeBuffer(Dots.TextureAttr(), GL_FLOAT, sizeof(decltype(vertices->position)), 2, sizeof(VertexData));

	Dots.ShaderProgram()->enableAttributeArray(Dots.ColorAttr());
	Dots.ShaderProgram()->setAttributeBuffer(Dots.ColorAttr(), GL_FLOAT, sizeof(decltype(vertices->position)) + sizeof(decltype(vertices->texCoord)), 3, sizeof(VertexData));

	Dots.Ebo().allocate(indices, iSize * sizeof(GLushort));
	Dots.releaseAll();

	delete[] vertices;
	delete[] indices;
}