#include "SceneCamera.h"

SceneCamera::SceneCamera()
{
	m_perspective.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	m_perspective.translate(0, 0, -2);
}

SceneCamera::~SceneCamera()
{
}

void SceneCamera::rotate(float angle, const QVector3D& direction)
{
	m_perspective.rotate(angle, direction);
}