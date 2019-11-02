#include "graphicsPanel.h"
#include "TopWindow.h"
#include <QVector2D>
#include <QVector3D>
#include <QPainter>
#include <QWheelEvent>
#include "ShapeCreator.h"
#include <QtMath>
#include "utils.h"
#include <fstream>

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
	//m_figureRenderer = new FigureRenderer(this, "../N&I_rig.fbx");
	//m_figureRenderer = new FigureRenderer(this, "../modelLoadingTest.fbx");
	m_figureRenderer = new FigureRenderer(this, "../modeltest3.fbx");
	//m_figureRenderer = new FigureRenderer(this, "../cylinderTest2.fbx");

	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);
}

void GraphicsPanel::setBackground(QVector4D background)
{
	glClearColor(background.x(), background.y(), background.z(), background.w());
}

void GraphicsPanel::myPaint()
{
	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartingTime);

	QPainter painter;
	painter.begin(this);
	painter.beginNativePainting();

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	QMatrix4x4 model;

	//m_floorRenderer->setMVP(model, m_camera.View(), m_camera.Perspective());
	//m_floorRenderer->Draw();
	
	m_figureRenderer->setMVP(model, m_camera.View(), m_camera.Perspective());

	if (m_frame % 1 == 0)
	{
		m_figureRenderer->getMeshManager().animate();
		m_figureRenderer->getMeshManager().getMeshes()[0]->initializeBuffers();
	}
	m_figureRenderer->Draw();
	
	updateFrameCt(m_figureRenderer->getMeshManager().getFrameCt());
	painter.end();

	++m_frame;

	if (!m_paused)
		update();

	// Activity to be timed

	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;

	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
	qDebug() << ElapsedMicroseconds.QuadPart;
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
	if (m_middlePressed)
	{
		if(abs(event->pos().x() - m_lastPos.x()) > abs(event->pos().y() - m_lastPos.y()))
			m_camera.setYaw(m_camera.Yaw() + (event->pos().x() > m_lastPos.x() ? 1.0 : -1.0));
		else
			m_camera.setPitch(m_camera.Pitch() + (event->pos().y() > m_lastPos.y() ? 1.0 : -1.0));
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
	bool ctrlPress = false;
	if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
		shiftPress = true;
	if (QGuiApplication::keyboardModifiers() & Qt::ControlModifier)
		ctrlPress = true;
	switch (event->key())
	{
	case Qt::Key_Up:
		if (shiftPress)
			m_camera.rotateCam(Direction::Up);
		else if (ctrlPress)
			m_camera.moveCam(Direction::Up);
		else
			m_camera.moveCam(Direction::Forward);
		break;
	case Qt::Key_Down:
		if (shiftPress)
			m_camera.rotateCam(Direction::Down);
		else if(ctrlPress)
			m_camera.moveCam(Direction::Down);
		else
			m_camera.moveCam(Direction::Backwards);
		break;
	case Qt::Key_Left:
		shiftPress ? m_camera.rotateCam(Direction::Left) :
		m_camera.moveCam(Direction::Left);
		break;
	case Qt::Key_Right:
		shiftPress ? m_camera.rotateCam(Direction::Right) :
		m_camera.moveCam(Direction::Right);
		break;
	case Qt::Key_A:
		m_camera.rollCam(Direction::Rotation::CCW);
		break;
	case Qt::Key_D:
		m_camera.rollCam(Direction::Rotation::CW);
		break;
	case Qt::Key_Space:
		m_camera.resetView();
		break;
	case Qt::Key_1:
		m_camera.moveCamPlane(Direction::Plane::East);
		break;
	case Qt::Key_2:
		m_camera.moveCamPlane(Direction::Plane::West);
		break;
	case Qt::Key_3:
		m_camera.moveCamPlane(Direction::Plane::Top);
		break;
	case Qt::Key_4:
		m_camera.moveCamPlane(Direction::Plane::Bottom);
		break;
	case Qt::Key_5:
		m_camera.moveCamPlane(Direction::Plane::Front);
		break;
	case Qt::Key_6:
		m_camera.moveCamPlane(Direction::Plane::Back);
		break;
	case Qt::Key_Equal:
		m_figureRenderer->getMeshManager().incrementFrame();
		break;
	case Qt::Key_Minus:
		m_figureRenderer->getMeshManager().decrementFrame();
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

void GraphicsPanel::updateFrameCt(int value)
{
	((TopWindow*)m_parent)->updateFrameCt(value);
}