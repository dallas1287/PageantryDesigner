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
	//m_figureRenderer = new FigureRenderer(this, "../N&I_rig.fbx");
	m_figureRenderer = new FigureRenderer(this, "../modelLoadingTest.fbx");
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

	QMatrix4x4 model;
	//model.setToIdentity();
	//model.rotate(270, X);
	
	m_floorRenderer->setMVP(model, m_camera.View(), m_camera.Perspective());
	m_floorRenderer->Draw();

	model.scale(.1);
	m_figureRenderer->setMVP(model, m_camera.View(), m_camera.Perspective());
	//move bone test
	m_figureRenderer->getMeshManager().getMeshes()[0]->moveBone("Bone.003", QVector3D(15.0, 0.0, 0.0));
	m_figureRenderer->getMeshManager().getMeshes()[0]->initializeBuffers();
	m_figureRenderer->Draw();

	//m_figureRenderer->getMeshManager().getMeshes()[0]->getBoneRig().MeshTransformTest();

	/*model.setToIdentity();
	model.scale(.05);
	model.translate(1.01 * Z);

	for (int i = 0; i < 3; ++i)
	{
		model.translate(2.0* (X + Y));
		m_dotsRenderer->setMVP(model, m_camera.View(), m_camera.Perspective());
		m_dotsRenderer->Draw();
	}*/
	
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
	case Qt::Key_1:
		m_camera.moveCamPlaneLeft();
		break;
	case Qt::Key_3:
		m_camera.moveCamPlaneRight();
		break;
	case Qt::Key_5:
		m_camera.moveCamPlaneTop();
		break;
	case Qt::Key_7:
		m_camera.moveCamPlaneBottom();
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