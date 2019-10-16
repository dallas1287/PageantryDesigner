#include "graphicsPanel.h"
#include "TopWindow.h"
#include <QVector2D>
#include <QVector3D>
#include <QPainter>
#include <QWheelEvent>
#include "ShapeCreator.h"
#include <QtMath>

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
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	setFocus(Qt::FocusReason::OtherFocusReason);
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
	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);

}

void GraphicsPanel::setBackground(QVector4D background)
{
	glClearColor(background.x(), background.y(), background.z(), background.w());
}

static float count = 0.0;
static bool dir = true;
void GraphicsPanel::myPaint()
{
	QPainter painter;
	painter.begin(this);
	painter.beginNativePainting();

	//glFrontFace(GL_CW);
	//glCullFace(GL_FRONT);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*if (m_frame % 3 == 0)
	{
		float newX = qSin(m_frame/3) * 3;
		float newZ = qCos(m_frame/3) * 3;
		m_camera.setPosition(QVector3D(newX, 2.0, newZ));
	}*/
	QMatrix4x4 model;
	model.setToIdentity();
	model.rotate(270, X);
	
	m_floorRenderer->setMVP(model, m_camera.View(), m_camera.Perspective());
	m_floorRenderer->Draw();

	m_figureRenderer->setMVP(model, m_camera.View(), m_camera.Perspective());
	m_figureRenderer->Draw();

	model.setToIdentity();
	model.scale(.05);
	model.translate(1.01 * Z);

	for (int i = 0; i < 3; ++i)
	{
		model.translate(2.0* (X + Y));
		m_dotsRenderer->setMVP(model, m_camera.View(), m_camera.Perspective());
		m_dotsRenderer->Draw();
	}
	
	painter.end();

	++m_frame;

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
	if (event->angleDelta().y() > 0)
		m_camera.zoomIn();
	else if (event->angleDelta().y() < 0)
		m_camera.zoomOut();
}

void GraphicsPanel::mouseMoveEvent(QMouseEvent* event)
{
	//screen coordinates for mouse are X - left to right & Y - up and down
	//rotating around the x axis moves up and down and around the y axis moves left to right
	//so the mouse y corresponds to an X rotation of the camera and vice versa
	if (m_middlePressed)
	{
		//event->pos().y() > m_lastPos.y() ? m_camera.setPosition(m_camera.Position() + (.01 * Y)) : m_camera.setPosition(m_camera.Position() + (-.01 * Y));
		//event->pos().x() > m_lastPos.x() ? m_camera.setPosition(m_camera.Position() + (-.01 * X)) : m_camera.setPosition(m_camera.Position() + (.01 * X));

		int xChange = event->pos().x() - m_lastPos.x();

		float newX = qSin(xChange);
		float newZ = qCos(xChange);
		m_camera.setPosition(m_camera.Position() + QVector3D(newX, 0.0, newZ));
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

void GraphicsPanel::keyPressEvent(QKeyEvent* event)
{
	bool shiftPress = false;
	if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
		shiftPress = true;
	switch (event->key())
	{
	case Qt::Key_Up:
		shiftPress ? m_camera.setPitch(m_camera.Pitch() + 1.0) :
			m_camera.setPosition(m_camera.Position() + QVector3D(0.0, 0.0, -0.05));
		break;
	case Qt::Key_Down:
		shiftPress ? m_camera.setPitch(m_camera.Pitch() - 1.0) :
			m_camera.setPosition(m_camera.Position() + QVector3D(0.0, 0.0, 0.05));
		break;
	case Qt::Key_Left:
		shiftPress ? m_camera.setYaw(m_camera.Yaw() - 1.0) :
		m_camera.setPosition(m_camera.Position() + QVector3D(-0.05, 0.0, 0.0));
		break;
	case Qt::Key_Right:
		shiftPress ? m_camera.setYaw(m_camera.Yaw() + 1.0) :
		m_camera.setPosition(m_camera.Position() + QVector3D(0.05, 0.0, 0.0));
		break;
	case Qt::Key_A:
		m_camera.setRoll(m_camera.Roll() + 1.0);
		break;
	case Qt::Key_D:
		m_camera.setRoll(m_camera.Roll() - 1.0);
		break;
	case Qt::Key_Space:
		m_camera.resetView();
		break;
	default:
		return;
	}
}

void GraphicsPanel::keyReleaseEvent(QKeyEvent* event)
{

	switch (event->key())
	{
	case Qt::Key_Down:
		break;
	case Qt::Key_Up:
		break;
	case Qt::Key_Left:
		break;
	case Qt::Key_Right:
		break;
	default:
		return;
	}
}