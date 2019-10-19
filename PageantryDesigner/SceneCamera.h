#pragma once
#include <QMatrix4x4>
#include <QtMath>

const QVector3D DefaultPosition(0.0, 0.0, 3.0);
const QVector3D DefaultTarget(0.0, 0.0, 0.0);
const QVector3D DefaultCamUp(0.0, 1.0, 0.0);
const QVector3D DefaultCamFront(0.0, 0.0, -1.0);
const float DefaultYaw = -90.0;
const float DefaultPitch = 0.0;
const float DefaultRoll = 90.0;
const float DefaultFov = 60.0;
const float DefaultAspectRatio = (4.0 / 3.0);
const float DefaultNearPlane = 0.1;
const float DefaultFarPlane = 100.0;

namespace Direction
{
	enum Movement
	{
		Forward,
		Backwards,
		Left,
		Right,
		Up,
		Down
	};

	enum Plane
	{
		Front,
		Back,
		West,
		East,
		Top,
		Bottom
	};

	enum Rotation
	{
		CW,
		CCW
	};

	enum Axis
	{
		Xaxis,
		Yaxis,
		Zaxis
	};
}

class SceneCamera
{
public:

	SceneCamera();
	~SceneCamera();

	QVector3D& Position() { return m_position; }
	void setPosition(const QVector3D& vector) { m_position = vector; updateView(); }
	QVector3D& Target() { return m_lookTarget; }
	void setTarget(const QVector3D& vector) { m_lookTarget = vector; }

	QVector3D& Front() { return m_camFront; }
	void setFront(QVector3D& vector) { m_camFront = vector; }
	void updateCamFront();

	QVector3D& Up() { return m_camUp; }
	void setCamUp(const QVector3D& vector) { m_camUp = vector; updateView(); }
	void updateCamUp();

	QMatrix4x4& View() { return m_camView; }
	void updateView();
	void resetView();

	QVector3D getForwardVector();
	QVector3D getRightVector();

	void moveCam(Direction::Movement dir);
	void rollCam(Direction::Rotation rot);
	void rotateCam(Direction::Movement dir);
	void moveCamPlane(Direction::Plane plane);

	const float Yaw() const { return m_yaw; }
	void setYaw(const float& val);
	const float Pitch() const { return m_pitch; }
	void setPitch(const float& val);
	const float Roll() const { return m_roll; }
	void setRoll(const float& val);
	void resetYPW();

	QMatrix4x4& Perspective() { return m_perspective; }
	void setPerspective(QMatrix4x4& matrix) { m_perspective.setToIdentity(); m_perspective = matrix; }
	void zoomIn();
	void zoomOut();

private:
	QVector3D m_position;
	QVector3D m_lookTarget;
	QVector3D m_camUp;
	QVector3D m_camFront;
	QMatrix4x4 m_camView;
	QMatrix4x4 m_perspective;

	float m_yaw = DefaultYaw;
	float m_pitch = DefaultPitch;
	float m_roll = DefaultRoll;
	float m_fov = DefaultFov;
};

