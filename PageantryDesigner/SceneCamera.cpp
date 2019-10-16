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
	
	m_yaw = DefaultYaw;
	m_pitch = DefaultPitch;
	m_roll = DefaultRoll;
	m_fov = DefaultFov;

	updateView();
}

void SceneCamera::updateView()
{
	updateCamFront();
	m_lookTarget = m_position + m_camFront;
	m_camView.setToIdentity();
	m_camView.lookAt(m_position, m_lookTarget, m_camUp);
}

void SceneCamera::updateCamFront()
{
	m_camFront = QVector3D(qCos(qDegreesToRadians(m_pitch)) * cos(qDegreesToRadians(m_yaw)),
		qSin(qDegreesToRadians(m_pitch)),
		qCos(qDegreesToRadians(m_pitch)) * qSin(qDegreesToRadians(m_yaw)));
	m_camFront.normalize();
	m_camUp = QVector3D(qCos(qDegreesToRadians(m_roll)), qSin(qDegreesToRadians(m_roll)), 0.0);
	m_camUp.normalize();
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