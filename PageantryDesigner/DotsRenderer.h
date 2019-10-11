#pragma once
#include "ItemRenderer.h"

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
	virtual void setupAttributes() override;

private:


	QOpenGLShaderProgram* m_program;
	QOpenGLTexture* m_texture;
};

