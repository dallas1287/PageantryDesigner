#include "PrimitiveObject.h"
#include "utils.h"

PrimitiveObject::PrimitiveObject() : GraphicsObject()
{
}

PrimitiveObject::PrimitiveObject(enum Primitive::Type type) : GraphicsObject()
{
	switch (type)
	{
	case Primitive::Type::Quad:
		generateQuad();
		break;
	case Primitive::Type::Cube:
		generateCube();
		break;
	default:
		break;
	}
}

PrimitiveObject::~PrimitiveObject()
{
}

void PrimitiveObject::initialize()
{
	initShaders("mesh_vertex.glsl", "multiLight_frag.glsl");
	setupBuffers(m_vertexData, m_indices);
	//initBuffers(m_vertexData, m_indices);
}

void PrimitiveObject::setType(int type)
{
	if (type > Primitive::Type::Undefined || type < Primitive::Type::Quad)
		return;

	m_type = type;
}

void PrimitiveObject::generateQuad()
{
	ShapeMaker::Quad::createQuad(m_vertexData, m_indices);
	m_type = Primitive::Type::Quad;
	initialize();
}

void PrimitiveObject::generateCube()
{
	ShapeMaker::Cube::createCube(m_vertexData, m_indices);
	m_type = Primitive::Type::Cube;
	initialize();
}

void PrimitiveObject::resize(float scale)
{
	if (m_vertexData.empty())
		return;

	for (int i = 0; i <  m_vertexData.size(); ++i)
	{
		m_vertexData[i].position *= scale;
		m_vertexData[i].texCoord *= scale;
	}
	setupBuffers();
}