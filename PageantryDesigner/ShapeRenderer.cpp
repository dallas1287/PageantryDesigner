#include "ShapeRenderer.h"

ShapeRenderer::ShapeRenderer(GraphicsPanel* parent) : ItemRenderer(parent)
{
	initialize();
}

ShapeRenderer::~ShapeRenderer()
{
}

void ShapeRenderer::initialize()
{
	ItemRenderer::initialize();
	//initShaders();
	//initTextures("../tattoo_comic.jpg");
	initializeBuffers();
}

void ShapeRenderer::setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection)
{

}

void ShapeRenderer::initializeBuffers()
{

}

void ShapeRenderer::Draw()
{

}