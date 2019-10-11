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
	void setBackground(QVector4D background);
	void myPaint();

	QWidget* m_parent = nullptr;
	int m_frame = 0;
	bool m_paused = false;
	FloorRenderer* m_floorRenderer = nullptr;
};

