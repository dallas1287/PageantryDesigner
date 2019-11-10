#pragma once
#include "RendererBase.h"
#include "GraphicsObject.h"

class FloorRenderer : public RendererBase
{
public:
	FloorRenderer(GraphicsPanel* parent);
	~FloorRenderer();
	virtual void setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection) override;
	virtual void Draw() override;

private:
	virtual void initialize() override;
	void initShaders();
	void initTextures(const QString& path);
	void generateFloor();
	void drawFloor();
	int generateGridLines(int lineCount);
	void drawGridLines();

	GraphicsObject Floor;
	GraphicsObject Grid;

	int m_gridLineCt = 0;
};

