#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "ShapeCreator.h"

class GraphicsPanel;

class FloorRenderer : protected QOpenGLFunctions
{
public:
	FloorRenderer(GraphicsPanel* parent);
	~FloorRenderer();

	void Draw();

	void setMatrix(QMatrix4x4 matrix);
	const QOpenGLShaderProgram* FloorShaderProgram() const { return m_floorProgram; }
	const QOpenGLShaderProgram* FloorGridShaderProgram() const { return m_gridProgram; }

private:
	void initialize();
	void initShaders();
	void initTexture(const QString& path);
	void setupAttributes();
	void drawBuffers();
	void drawFloor();
	void drawGridLines();
	
	GraphicsPanel* m_parent;

	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo, m_ebo;
	QOpenGLShaderProgram* m_floorProgram = nullptr;
	QOpenGLTexture* m_texture = nullptr;
	QOpenGLBuffer m_arrayBuf, m_indexBuf;
	GLuint m_posAttr, m_colAttr, m_texCoordAttr, m_matrixUniform;

	QOpenGLShaderProgram* m_gridProgram = nullptr;
	GLuint m_gridposAttr, m_gridcolAttr, m_gridmatrixUniform;
	VertexData* m_gridData = nullptr;
	GLushort* m_gridIndices = nullptr;
};

