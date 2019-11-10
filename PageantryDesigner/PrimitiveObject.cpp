#include "PrimitiveObject.h"
#include "utils.h"

PrimitiveObject::PrimitiveObject() : GraphicsObject()
{
}

PrimitiveObject::~PrimitiveObject()
{
}

void PrimitiveObject::generateQuad()
{
	ShapeMaker::Quad::createQuad(m_vertexData, m_indices);
}

void PrimitiveObject::generateCube()
{
	ShapeMaker::Cube::createCube(m_vertexData, m_indices);
}