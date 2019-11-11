#pragma once
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

struct VertexData;
class GraphicsPanel;

class RendererBase : protected QOpenGLFunctions
{
public:
	RendererBase();
	RendererBase(GraphicsPanel* parent);
	virtual ~RendererBase();
	virtual void setMVP(QMatrix4x4& model, QMatrix4x4& view, QMatrix4x4& projection) = 0;
	virtual void Draw() = 0;
	GraphicsPanel* getParent() { return m_parent; }

protected:
	virtual void initialize();

	GraphicsPanel* m_parent;
};

