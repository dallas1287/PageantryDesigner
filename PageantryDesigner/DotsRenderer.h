#pragma once
#include "ItemRenderer.h"
#include "GraphicsObject.h"

class DotsRenderer : public ItemRenderer
{
public:
	DotsRenderer(GraphicsPanel* parent);
	~DotsRenderer();
	virtual void Draw() override;

protected:
	virtual void initialize() override;
	virtual void initShaders() override;
	virtual void initTextures(const QString& path) override;
	void generateDots();

private:
	GraphicsObject Dots;
};

