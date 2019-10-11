#pragma once
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

class GraphicsPanel;
struct VertexData;

class ItemRenderer : protected QOpenGLFunctions
{
public:
	ItemRenderer(GraphicsPanel* parent);
	virtual ~ItemRenderer();
	virtual void Draw() = 0;

protected:
	virtual void initialize() = 0;
	virtual void initShaders() = 0;
	virtual void initTextures(const QString& path) = 0;
	virtual void setupAttributes() = 0;

	GraphicsPanel* m_parent;

	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo, m_ebo;
	QOpenGLTexture* m_texture = nullptr;
	GLuint m_posAttr, m_colAttr, m_texCoordAttr, m_matrixUniform;
};

