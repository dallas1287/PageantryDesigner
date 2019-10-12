#pragma once
#include "ItemRenderer.h"
#include "GraphicsObject.h"

class FloorRenderer : public ItemRenderer
{
public:
	FloorRenderer(GraphicsPanel* parent);
	~FloorRenderer();
	void setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection);

	void Draw();

private:
	virtual void initialize() override;
	virtual void initShaders() override;
	virtual void initTextures(const QString& path) override;
	void generateFloor();
	void drawFloor();
	int generateGridLines(int lineCount);
	void drawGridLines();
	void generateBuffers();
	void drawBuffers();

	GraphicsObject Floor;
	GraphicsObject Grid;

	int m_gridLineCt = 0;
};

