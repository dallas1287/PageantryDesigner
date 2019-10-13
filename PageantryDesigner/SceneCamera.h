#pragma once
#include <QMatrix4x4>

class SceneCamera
{
public:
	SceneCamera();
	~SceneCamera();
	QMatrix4x4 Perspective() { return m_perspective; }
	void setPerspective(QMatrix4x4& matrix) { m_perspective = matrix; }

private:
	QMatrix4x4 m_perspective;
};

