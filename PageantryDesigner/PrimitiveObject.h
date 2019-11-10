#pragma once
#include "GraphicsObject.h"

class PrimitiveObject : public GraphicsObject
{
public:
	PrimitiveObject();
	~PrimitiveObject();
	void generateQuad();
	void generateCube();

};

