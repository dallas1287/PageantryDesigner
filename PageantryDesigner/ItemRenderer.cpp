#include "ItemRenderer.h"

ItemRenderer::ItemRenderer()
{
	initialize();
}

ItemRenderer::ItemRenderer(GraphicsPanel* parent) : m_parent(parent)
{
	initialize();
}

ItemRenderer::~ItemRenderer()
{
}

void ItemRenderer::initialize()
{
	initializeOpenGLFunctions();
}