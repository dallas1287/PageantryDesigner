#include "SceneCamera.h"
#include "utils.h"
#include <QtMath>

SceneCamera::SceneCamera(): m_position(DefaultPosition), m_lookTarget(DefaultTarget), m_camUp(DefaultCamUp), m_baseCamUp(DefaultCamUp), m_camFront(DefaultCamFront), m_baseCamFront(DefaultCamFront)
{
	m_perspective.perspective(DefaultFov, DefaultAspectRatio, DefaultNearPlane, DefaultFarPlane);
	updateView();
}

SceneCamera::SceneCamera(aiCamera* refCam) : m_refCam(refCam)
{
	initFromImport();
}

SceneCamera::~SceneCamera()
{
}

void SceneCamera::initFromImport()
{
	if (!m_refCam)
		return;

	m_name = QString(m_refCam->mName.C_Str());
	m_aspect = m_refCam->mAspect;
	m_clipPlaneFar = m_refCam->mClipPlaneFar;
	m_clipPlaneNear = m_refCam->mClipPlaneNear;
	m_horizontalFov = m_refCam->mHorizontalFOV;
	m_lookTarget = convertAiToVector(m_refCam->mLookAt);
	m_position = convertAiToVector(m_refCam->mPosition);
	m_camUp = convertAiToVector(m_refCam->mUp);
}

void SceneCamera::resetView()
{
	m_position = DefaultPosition;
	m_lookTarget = DefaultTarget;
	m_camFront = DefaultCamFront;
	m_camUp = DefaultCamUp;
	m_baseCamFront = DefaultCamFront;
	m_baseCamUp = DefaultCamUp;
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
	QQuaternion pitchRotation = QQuaternion::fromAxisAndAngle(getRightVector(), m_pitch);
	QQuaternion yawRotation = QQuaternion::fromAxisAndAngle(m_camUp, m_yaw);
	QQuaternion rollRotation = QQuaternion::fromAxisAndAngle(m_camFront, m_roll);
	QQuaternion total = pitchRotation * yawRotation * rollRotation;
	setFront(total.rotatedVector(m_baseCamFront));
	m_camFront.normalize();
}

void SceneCamera::updateCamUp()
{
	QQuaternion pitchRotation = QQuaternion::fromAxisAndAngle(getRightVector(), m_pitch);
	QQuaternion yawRotation = QQuaternion::fromAxisAndAngle(m_camUp, m_yaw);
	QQuaternion rollRotation = QQuaternion::fromAxisAndAngle(m_camFront, m_roll);
	QQuaternion total = pitchRotation * yawRotation * rollRotation;
	setUp(total.rotatedVector(m_baseCamUp));
	m_camUp.normalize();
}

QVector3D SceneCamera::getForwardVector()
{
	return QVector3D(Target().x() - Position().x(), Target().y() - Position().y(), Target().z() - Position().z());
}

QVector3D SceneCamera::getRightVector()
{
	return QVector3D::crossProduct(m_camFront, m_camUp);
}

QVector3D SceneCamera::getDefaultRightVector()
{
	return QVector3D::crossProduct(DefaultCamFront, DefaultCamUp);
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
	updateView();
}

void SceneCamera::rotateCam(Direction::Movement dir)
{
	switch (dir)
	{
	case Direction::Left:
		setYaw(Yaw() + 1.0);
		break;
	case Direction::Right:
		setYaw(Yaw() - 1.0);
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
	updateView();
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
	updateView();
}

void SceneCamera::moveCamPlane(Direction::Plane plane)
{
	resetYPW();
	switch (plane)
	{
	case Direction::Front:
		setPosition(DefaultPosition);
		setYaw(DefaultYaw);
		setBaseFront(DefaultCamFront);
		setBaseUp(DefaultCamUp);
		break;
	case Direction::Back:
		setPosition(QVector3D(0.0, 0.0, -3.0));
		setYaw(180);
		setBaseFront(DefaultCamFront);
		setBaseUp(DefaultCamUp);
		break;
	case Direction::West:
		setPosition(QVector3D(3.0, 0.0, 0.0));
		setYaw(90);
		setBaseFront(DefaultCamFront);
		setBaseUp(DefaultCamUp);
		break;
	case Direction::East:
		setPosition(QVector3D(-3.0, 0.0, 0.0));
		setYaw(-90);
		setBaseFront(DefaultCamFront);
		setBaseUp(DefaultCamUp);
		break;
	case Direction::Top:
		setPosition(QVector3D(0.0, 3.0, 0.0));
		setBaseFront(-1.0 * Y);
		setBaseUp(Z);
		break;
	case Direction::Bottom:
		setPosition(QVector3D(0.0, -3.0, 0.0));
		setBaseFront(Y);
		setBaseUp(Z);
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
	if (m_yaw <= -360)
		m_yaw = 0;
}

void SceneCamera::setPitch(const float& val)
{
	m_pitch = val;
	if (m_pitch >= 360)
		m_pitch = 0;
	if (m_pitch <= -360)
		m_pitch = 0;
}

void SceneCamera::setRoll(const float& val)
{
	m_roll = val;
	if (m_roll >= 360)
		m_roll = 0;
	if (m_roll <= -360)
		m_roll = 0;
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

QVector3D SceneCamera::roundVector(const QVector3D& vector)
{
	return QVector3D(roundFloat(vector.x()), roundFloat(vector.y()), roundFloat(vector.z()));
}

float SceneCamera::roundFloat(float value, int precision)
{
	int intermediate = value * qPow(10, precision);
	return (float)intermediate / qPow(10, precision);
}