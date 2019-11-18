#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include "MeshRenderer.h"
#include "SceneCamera.h"

#define USE_COLLADA 0

class GraphicsPanel : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
	Q_OBJECT
public:
	GraphicsPanel(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
	virtual ~GraphicsPanel();
	QWidget* getParent() { return m_parent; }
	SceneCamera& getCamera() { return m_camera; }
	void updateCameraStats();
	void updateFrameCt(int value);
	void setAnimationFrame(int value);
	bool isPaused() { return m_paused; }
	void onPlayClicked();
	void resetViewPort();
	MeshRenderer* getMeshRenderer() { return m_MeshRenderer.get(); }
	void populateAnimCb();
	void populateMeshesCb();
	void importModel(const QString& importPath);
	bool getBlinn() const { return m_blinn; }

public slots:
	void onAnimCbChanged(int index);

protected:
	virtual void wheelEvent(QWheelEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);
	virtual void initializeGL() override;
	virtual void paintGL() override;
	virtual void resizeGL(int w, int h) override;

private:
	void setBackground(QVector4D background);
	void myPaint();

	void initFrameBuffer();
	void writeToFrameBuffer();
	void renderDepthMap();

	void standardInitPlane();
	void renderScene(QOpenGLShaderProgram* program);
	void renderCube();

	void Animating();
	void DrawLighting();
	void tempLightSetup();
	bool m_blinn = false;

	QWidget* m_parent = nullptr;
	unsigned int m_frame = 0;
	bool m_paused = true;
	QPoint m_lastPos;
	bool m_middlePressed = false;
	SceneCamera m_camera;
	std::unique_ptr<MeshRenderer> m_MeshRenderer;
};

