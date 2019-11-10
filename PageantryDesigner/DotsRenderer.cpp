#include "DotsRenderer.h"
#include <QPainter>
#include "utils.h"

DotsRenderer::DotsRenderer(GraphicsPanel* parent) : RendererBase(parent)
{
	initialize();
}

DotsRenderer::~DotsRenderer()
{
}

void DotsRenderer::initialize()
{
	RendererBase::initialize();
	initShaders();
	initTexture("../flag.png");
	generateDots();
}

void DotsRenderer::initShaders()
{
	Dots.initShaders("dots_vertex.glsl", "dots_fragment.glsl");
}

void DotsRenderer::initTexture(const QString& path)
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
	std::vector<VertexData> vData;
	std::vector<GLushort> iData;
	ShapeMaker::Quad::createQuad(vData, iData);

	Dots.bindAll();
	Dots.Vbo().allocate(&vData[0], vData.size() * sizeof(VertexData));

	Dots.ShaderProgram()->enableAttributeArray(Dots.PosAttr());
	Dots.ShaderProgram()->setAttributeBuffer(Dots.PosAttr(), GL_FLOAT, 0, 3, sizeof(VertexData));

	Dots.ShaderProgram()->enableAttributeArray(Dots.TextureAttr());
	Dots.ShaderProgram()->setAttributeBuffer(Dots.TextureAttr(), GL_FLOAT, sizeof(decltype(vData[0].position)), 3, sizeof(VertexData));

	//TODO: correct this to account for norms and colors and other added vertex data
	Dots.ShaderProgram()->enableAttributeArray(Dots.ColorAttr());
	Dots.ShaderProgram()->setAttributeBuffer(Dots.ColorAttr(), GL_FLOAT, sizeof(decltype(vData[0].position)) + sizeof(decltype(vData[0].texCoord)), 3, sizeof(VertexData));

	Dots.Ebo().allocate(&iData[0], iData.size() * sizeof(GLushort));
	Dots.releaseAll();
}