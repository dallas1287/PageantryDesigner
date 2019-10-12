#pragma once
#include "ItemRenderer.h"
#include "GraphicsObject.h"

class DotsRenderer : public ItemRenderer
{
public:
	DotsRenderer(GraphicsPanel* parent);
	~DotsRenderer();
	virtual void Draw() override;
	void setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection);

protected:
	virtual void initialize() override;
	virtual void initShaders() override;
	virtual void initTextures(const QString& path) override;

private:
	void generateDots();

private:
	GraphicsObject Dots;
};

