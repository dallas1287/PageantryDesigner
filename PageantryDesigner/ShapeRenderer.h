#pragma once
#include "ItemRenderer.h"
#include "GraphicsObject.h"

class ShapeRenderer : public ItemRenderer
{
public:
	ShapeRenderer(GraphicsPanel* parent);
	virtual ~ShapeRenderer();
	virtual void setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection) override;
	virtual void Draw() override;

private:
	virtual void initialize() override;
	void initializeBuffers();
	GraphicsObject* m_shape;
};

