#include "graphicsPanel.h"
#include "TopWindow.h"
#include <QVector2D>
#include <QVector3D>
#include <QPainter>
#include <QWheelEvent>
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
};

void GraphicsPanel::importModel(const QString& importPath)
{
	m_MeshRenderer.reset(new MeshRenderer(this, importPath));
}

void GraphicsPanel::initializeGL()
{
	initializeOpenGLFunctions();
	setBackground(background);
	m_floorRenderer.reset(new FloorRenderer(this));
	//m_MeshRenderer.reset(new MeshRenderer(this, "../N&I_rig_baked.fbx"));
	//m_MeshRenderer = new MeshRenderer(this, "../modeltest7_multi.fbx");
	//m_MeshRenderer.reset(new MeshRenderer(this, "../cylinderTest2.fbx"));
	//m_MeshRenderer.reset(new MeshRenderer(this, "../kitty_new6.fbx"));
	//m_MeshRenderer.reset(new MeshRenderer(this, "../cube_texture_scene.fbx"));
	m_MeshRenderer.reset(new MeshRenderer(this, "../cube_color.fbx"));
	//m_MeshRenderer.reset(new MeshRenderer(this, "../color_sphere_uv.fbx"));
	//m_MeshRenderer.reset(new MeshRenderer(this, "../sphere_texture.fbx"));
	//m_MeshRenderer->initTextures("../cube_paint.png");
	//m_MeshRenderer->initTextures("../paint_sphere.png");

	populateAnimCb();
	populateMeshesCb();

	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);
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

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	QMatrix4x4 model;

	m_floorRenderer->setMVP(model, m_camera.View(), m_camera.Perspective());
	m_floorRenderer->Draw();

	m_MeshRenderer->setMVP(model, m_camera.View(), m_camera.Perspective());

	if (m_frame % 1 == 0)
	{
		m_MeshRenderer->getMeshManager()->animate();
		updateFrameCt(m_MeshRenderer->getMeshManager()->getFrameCt());
	}
	m_MeshRenderer->Draw();

	++m_frame;	
	painter.end();
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
		m_MeshRenderer->getMeshManager()->incrementFrame();
		break;
	case Qt::Key_Minus:
		m_MeshRenderer->getMeshManager()->decrementFrame();
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

void GraphicsPanel::onAnimCbChanged(int index)
{
	m_MeshRenderer->getMeshManager()->setCurrentAnimation(index);
}

void GraphicsPanel::populateAnimCb()
{
	if (!m_MeshRenderer.get())
		return;

	std::vector<QString> names;
	for (auto anim : m_MeshRenderer->getMeshManager()->getAnimations())
		names.push_back(anim->getName());
	((TopWindow*)m_parent)->populateAnimCb(names);
}

void GraphicsPanel::populateMeshesCb()
{
	if (!m_MeshRenderer.get())
		return;

	std::vector<QString> names;
	for (auto mesh : m_MeshRenderer->getMeshManager()->getMeshes())
		names.push_back(mesh->getName());
	((TopWindow*)m_parent)->populateMeshesCb(names);
}

void GraphicsPanel::setAnimationFrame(int value)
{
	if (value > 100 || value < 0)
		return;

	float frame = value * m_MeshRenderer->getMeshManager()->getCurrentAnimation()->getDuration() / 100;

	int final = qRound(frame);
	m_MeshRenderer->getMeshManager()->setFrameCt(final);
}