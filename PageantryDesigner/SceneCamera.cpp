#include "SceneCamera.h"
#include <QtMath>

SceneCamera::SceneCamera(): m_position(DefaultPosition), m_lookTarget(DefaultTarget), m_camUp(DefaultCamUp), m_camFront(DefaultCamFront)
{
	m_perspective.perspective(DefaultFov, DefaultAspectRatio, DefaultNearPlane, DefaultFarPlane);
	updateView();
}

SceneCamera::~SceneCamera()
{
}

void SceneCamera::resetView()
{
	m_position = DefaultPosition;
	m_lookTarget = DefaultTarget;
	m_camFront = DefaultCamFront;
	m_camUp = DefaultCamUp;
	m_perspective.setToIdentity();
	m_perspective.perspective(DefaultFov, DefaultAspectRatio, DefaultNearPlane, DefaultFarPlane);
	
	resetYPW();
	m_fov = DefaultFov;

	updateView();
}

void SceneCamera::resetYPW()
{
	m_yaw = DefaultYaw;
	m_pitch = DefaultPitch;
	m_roll = DefaultRoll;
}

void SceneCamera::updateView()
{
	updateCamFront();
	updateCamUp();
	m_lookTarget = m_position + m_camFront;
	m_camView.setToIdentity();
	m_camView.lookAt(m_position, m_lookTarget, m_camUp);
}

void SceneCamera::updateCamFront()
{
	m_camFront = QVector3D(qCos(qDegreesToRadians(m_pitch)) * qCos(qDegreesToRadians(m_yaw)),
		qSin(qDegreesToRadians(m_pitch)),
		qCos(qDegreesToRadians(m_pitch)) * qSin(qDegreesToRadians(m_yaw)));
	m_camFront.normalize();
}

void SceneCamera::updateCamUp()
{
	m_camUp = QVector3D(qCos(qDegreesToRadians(m_roll)),
		qSin(qDegreesToRadians(m_roll)) * qCos(qDegreesToRadians(m_pitch)),
		qSin(qDegreesToRadians(m_pitch)));
	m_camUp.normalize();
}

QVector3D SceneCamera::getForwardVector()
{
	return QVector3D(Target().x() - Position().x(), Target().y() - Position().y(), Target().z() - Position().z());
}

QVector3D SceneCamera::getRightVector()
{
	return QVector3D::crossProduct(getForwardVector(), m_camUp);
}


void SceneCamera::moveCam(Direction::Movement dir)
{
	switch (dir)
	{
	case Direction::Forward:
		setPosition(Position() + 0.05 * getForwardVector());
		break;
	case Direction::Backwards:
		setPosition(Position() + -0.05 * getForwardVector());
		break;
	case Direction::Left:
		setPosition(Position() + -0.05 * getRightVector());
		break;
	case Direction::Right:
		setPosition(Position() + 0.05 * getRightVector());
		break;
	case Direction::Up:
		setPosition(Position() + 0.05 * Up());
		break;
	case Direction::Down:
		setPosition(Position() + -0.05 * Up());
		break;
	default:
		return;
	}
}

void SceneCamera::rotateCam(Direction::Movement dir)
{
	switch (dir)
	{
	case Direction::Left:
		setYaw(Yaw() - 1.0);
		break;
	case Direction::Right:
		setYaw(Yaw() + 1.0);
		break;
	case Direction::Up:
		setPitch(Pitch() + 1.0);
		break;
	case Direction::Down:
		setPitch(Pitch() - 1.0);
		break;
	default:
		return;
	}
}

void SceneCamera::rollCam(Direction::Rotation rot)
{
	switch (rot)
	{
	case Direction::CW:
		setRoll(Roll() + 1.0);
		break;
	case Direction::CCW:
		setRoll(Roll() - 1.0);
		break;
	default:
		return;
	}
}

void SceneCamera::moveCamPlane(Direction::Plane plane)
{
	resetYPW();
	switch (plane)
	{
	case Direction::Front:
		setPosition(DefaultPosition);
		setYaw(DefaultYaw);
		break;
	case Direction::Back:
		setPosition(QVector3D(0.0, 0.0, -3.0));
		setYaw(90);
		break;
	case Direction::West:
		setPosition(QVector3D(3.0, 0.0, 0.0));
		setYaw(180);
		break;
	case Direction::East:
		setPosition(QVector3D(-3.0, 0.0, 0.0));
		setYaw(0);
		break;
	case Direction::Top:
		setPosition(QVector3D(0.0, 3.0, 0.0));
		setPitch(-90);
		break;
	case Direction::Bottom:
		setPosition(QVector3D(0.0, -3.0, 0.0));
		setPitch(90);
		break;
	default:
		return;
	}
	updateView();
}

void SceneCamera::setYaw(const float& val)
{
	m_yaw = val;
	if (m_yaw >= 360)
		m_yaw = 0;
	updateView();
}

void SceneCamera::setPitch(const float& val)
{
	m_pitch = val;
	if (m_pitch >= 360)
		m_pitch = 0;
	updateView();
}

void SceneCamera::setRoll(const float& val)
{
	m_roll = val;
	if (m_roll >= 360)
		m_roll = 0;
	updateView();
}

void SceneCamera::zoomIn()
{
	m_fov -= 1.0;
	if (m_fov < 1.0)
		m_fov = 1.0;

	m_perspective.setToIdentity();
	m_perspective.perspective(m_fov, DefaultAspectRatio, DefaultNearPlane, DefaultFarPlane);
}

void SceneCamera::zoomOut()
{
	m_fov += 1.0;
	m_perspective.setToIdentity();
	m_perspective.perspective(m_fov, DefaultAspectRatio, DefaultNearPlane, DefaultFarPlane);
}