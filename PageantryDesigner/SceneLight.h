#pragma once
#include "Includes/assimp/scene.h"
#include "SceneObject.h"
#include <QVector4D>
#include <QVector3D>

class SceneLight : public SceneObject
{
public:
	SceneLight() {}
	SceneLight(aiLight* refLight);
	~SceneLight() {}
	const QVector4D& getAmbient() { return m_ambientColor; }
	const QVector4D& getDiffuse() { return m_diffuseColor; }
	const QVector4D& getSpecular() { return m_specularColor; }
	const QVector3D& getDirection() { return m_direction; }
	const QVector3D& getPosition() { return m_position; }

private:
	void init();

	aiLight* m_refLight = nullptr;
	QVector4D m_ambientColor;
	QVector4D m_diffuseColor;
	QVector4D m_specularColor;
	QVector3D m_direction;
	QVector3D m_position;
	float m_attenuationConstant = 0.0;
	float m_attenuationLinear = 0.0;
	float m_attenuationQuadratic = 0.0;
};

