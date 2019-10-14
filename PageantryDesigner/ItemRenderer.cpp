#include "ItemRenderer.h"

ItemRenderer::ItemRenderer(GraphicsPanel* parent) : m_parent(parent)
{
}

ItemRenderer::~ItemRenderer()
{
}

void ItemRenderer::initialize()
{
	initializeOpenGLFunctions();
}