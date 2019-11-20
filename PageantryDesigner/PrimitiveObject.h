#pragma once
#include "GraphicsObject.h"

namespace Primitive
{
	enum Type
	{
		Quad,
		Cube,
		Undefined
	};
};

class PrimitiveObject : public GraphicsObject
{
public:
	PrimitiveObject();
	PrimitiveObject(enum Primitive::Type type);
	~PrimitiveObject();
	void initialize();
	void generateQuad();
	void generateCube();
	void resize(float scale);
	void setType(int type);
	int getType() { return m_type; }

private:
	int m_type = Primitive::Type::Undefined;

};

