#pragma once
#include "ItemRenderer.h"
#include "GraphicsObject.h"

class DotsRenderer : public ItemRenderer
{
public:
	DotsRenderer(GraphicsPanel* parent);
	~DotsRenderer();
	virtual void Draw() override;
	virtual void setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection) override;

protected:
	virtual void initialize() override;
	virtual void initShaders();
	virtual void initTexture(const QString& path);

private:
	void generateDots();

private:
	GraphicsObject Dots;
};

