#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "ShapeCreator.h"
#include "FloorRenderer.h"


class GraphicsPanel : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	GraphicsPanel(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
	virtual ~GraphicsPanel();
	void cleanup();

public slots:
	void onPlayClicked();

protected:
	virtual void wheelEvent(QWheelEvent* event);
	virtual void initializeGL() override;
	virtual void paintGL() override;
	virtual void resizeGL(int w, int h) override;

private:
	void initShaders();
	void initTexture(const QString& path);
	void setupAttributes();
	void createRectWithIndices();
	void drawLinesWithIndices();
	void setBackground(QVector4D background);
	void myPaint();
	void drawRectangle();
	void drawLines();

	friend class FloorRenderer;

	QWidget* m_parent = nullptr;
	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo, m_ebo;
	QOpenGLShaderProgram* m_program = nullptr;
	QOpenGLTexture* m_texture = nullptr;
	QOpenGLBuffer m_arrayBuf, m_indexBuf;
	GLuint m_posAttr, m_colAttr, m_matrixUniform;
	GLuint m_vertexAttr, m_texCoordAttr, m_texUniform;
	int m_frame = 0;
	bool m_paused = false;

	QOpenGLShaderProgram* m_gridProgram = nullptr;
	GLuint m_gridposAttr, m_gridcolAttr, m_gridmatrixUniform;
	VertexData* m_gridData = nullptr;
	GLushort* m_gridIndices = nullptr;

	FloorRenderer* m_floorRenderer = nullptr;
};

