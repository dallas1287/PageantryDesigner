#include "ItemRenderer.h"

ItemRenderer::ItemRenderer(GraphicsPanel* parent) : m_parent(parent), m_ebo(QOpenGLBuffer(QOpenGLBuffer::IndexBuffer))
{
}

ItemRenderer::~ItemRenderer()
{
	m_vbo.destroy();
	m_ebo.destroy();
	m_vao.destroy();
}