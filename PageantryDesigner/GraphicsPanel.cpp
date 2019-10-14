#include "graphicsPanel.h"
#include "TopWindow.h"
#include <QVector2D>
#include <QVector3D>
#include <QPainter>
#include <QWheelEvent>
#include "ShapeCreator.h"

const QVector4D background = { (float)135 / 255, (float)206 / 255, (float)250 / 255, 1.0f };

GraphicsPanel::GraphicsPanel(QWidget* parent, Qt::WindowFlags flags) : QOpenGLWidget(parent, flags), m_parent(parent)
{
	/*QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setMajorVersion(4);
	format.setMinorVersion(3);
	format.setSamples(4);
	format.setProfile(QSurfaceFormat::CoreProfile);
	setFormat(format);*/
}

GraphicsPanel::~GraphicsPanel()
{
	makeCurrent();

	delete m_floorRenderer;
	delete m_dotsRenderer;
	delete m_figureRenderer;

	doneCurrent();
};

void GraphicsPanel::initializeGL()
{
	initializeOpenGLFunctions();
	setBackground(background);
	m_floorRenderer = new FloorRenderer(this);
	m_dotsRenderer = new DotsRenderer(this);
	m_figureRenderer = new FigureRenderer(this, "../N&I_rig.fbx");
}

void GraphicsPanel::setBackground(QVector4D background)
{
	glClearColor(background.x(), background.y(), background.z(), background.w());
}

void GraphicsPanel::myPaint()
{
	QPainter painter;
	painter.begin(this);
	painter.beginNativePainting();

	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//projection.rotate(m_frame, 0, 1, 0);
	
	QMatrix4x4 model;
	model.setToIdentity();
	QMatrix4x4 view;
	view.setToIdentity();

	m_figureRenderer->setMVP(model, view, m_camera.Perspective());
	m_figureRenderer->Draw();

	m_floorRenderer->setMVP(model, view, m_camera.Perspective());
	m_floorRenderer->Draw();

	model.scale(.05);
	model.translate(1.01 * Z);

	for (int i = 0; i < 3; ++i)
	{
		model.translate(2.0* (X + Y));
		m_dotsRenderer->setMVP(model, view, m_camera.Perspective());
		m_dotsRenderer->Draw();
	}
	
	painter.end();

	++m_frame;
	if (m_frame > 360)
		m_frame = 0;

	if (!m_paused)
		update();
}

void GraphicsPanel::paintGL()
{
	myPaint();
}

void GraphicsPanel::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}

void GraphicsPanel::onPlayClicked()
{
	m_paused ^= true;
	if (!m_paused)
		myPaint();
}

void GraphicsPanel::wheelEvent(QWheelEvent* event)
{
	QMatrix4x4 camPersp = m_camera.Perspective();
	if (event->angleDelta().y() > 0)
	{
		camPersp.translate(Z * .01);
		m_camera.setPerspective(camPersp);
	}
	else if (event->angleDelta().y() < 0)
	{
		camPersp.translate(Z * -.01);
		m_camera.setPerspective(camPersp);
	}
}

void GraphicsPanel::mouseMoveEvent(QMouseEvent* event)
{
	qDebug() << "Last: " << m_lastPos << " New: " << event->pos();
	bool xGreater = abs(event->pos().x() - m_lastPos.x()) > abs(event->pos().y() - m_lastPos.y());

	//screen coordinates for mouse are X - left to right & Y - up and down
	//rotating around the x axis moves up and down and around the y axis moves left to right
	//so the mouse y corresponds to an X rotation of the camera and vice versa
	if (m_middlePressed)
	{
		if(!xGreater)
			event->pos().y() > m_lastPos.y() ? m_camera.rotate(1.0, X) : m_camera.rotate(1.0, -1.0 * X);
		else
			event->pos().x() > m_lastPos.x() ? m_camera.rotate(1.0, Y) : m_camera.rotate(1.0, -1.0 * Y);
		m_lastPos = event->pos();
	}
}

void GraphicsPanel::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MouseButton::MiddleButton)
	{
		m_middlePressed = true;
		m_lastPos = event->pos();
	}
}

void GraphicsPanel::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MouseButton::MiddleButton)
		m_middlePressed = false;
}