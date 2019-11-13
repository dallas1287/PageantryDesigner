#include "PrimitiveObject.h"
#include "utils.h"

PrimitiveObject::PrimitiveObject() : GraphicsObject()
{
}

PrimitiveObject::~PrimitiveObject()
{
}

void PrimitiveObject::initialize()
{
	initShaders("mesh_vertex.glsl", "color_frag.glsl");
	initBuffers(m_vertexData, m_indices);
}

void PrimitiveObject::generateQuad()
{
	ShapeMaker::Quad::createQuad(m_vertexData, m_indices);
	initialize();
}

void PrimitiveObject::generateCube()
{
	ShapeMaker::Cube::createCube(m_vertexData, m_indices);
	initialize();
}