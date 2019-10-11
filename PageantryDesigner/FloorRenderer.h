#pragma once
#include "ItemRenderer.h"

class FloorRenderer : public ItemRenderer
{
public:
	FloorRenderer(GraphicsPanel* parent);
	~FloorRenderer();

	void Draw();
	void setMatrix(QMatrix4x4 matrix);

private:
	virtual void initialize() override;
	virtual void initShaders() override;
	virtual void initTextures(const QString& path) override;
	virtual void setupAttributes() override;
	void generateFloor();
	void drawFloor();
	int generateGridLines(int lineCount);
	void drawGridLines();
	void generateBuffers();
	void drawBuffers();

	QOpenGLShaderProgram* m_floorProgram = nullptr;
	QOpenGLTexture* m_texture = nullptr;

	QOpenGLShaderProgram* m_gridProgram = nullptr;
	QOpenGLVertexArrayObject m_gridVao;
	QOpenGLBuffer m_vbo, m_gridEbo;
	GLuint m_gridposAttr, m_gridcolAttr, m_gridmatrixUniform;
	int m_gridLineCt = 0;

	//FloorData m_FloorData;
};

