#include "RendererBase.h"

RendererBase::RendererBase()
{
	initialize();
}

RendererBase::RendererBase(GraphicsPanel* parent) : m_parent(parent)
{
	initialize();
}

RendererBase::~RendererBase()
{
}

void RendererBase::initialize()
{
	initializeOpenGLFunctions();
}