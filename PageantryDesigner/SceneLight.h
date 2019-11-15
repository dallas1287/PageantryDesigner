#pragma once
#include "Includes/assimp/scene.h"
#include "SceneObject.h"
#include "common.h"
#include <QVector4D>
#include <QVector3D>

class SceneLight : public SceneObject
{
public:
	SceneLight() {}
	SceneLight(aiLight* refLight);
	~SceneLight() {}
	const QVector3D& getAmbient() { return m_lightData.ambient; }
	void setAmbient(const QVector3D& amb) { m_lightData.ambient = amb; }
	const QVector3D& getDiffuse() { return m_lightData.diffuse; }
	void setDiffuse(const QVector3D& dif) { m_lightData.diffuse = dif; }
	const QVector3D& getSpecular() { return m_lightData.specular; }
	void setSpecular(const QVector3D& spec) { m_lightData.specular = spec; }
	const QVector3D& getDirection() { return m_lightData.direction; }
	void setDirection(const QVector3D& dir) { m_lightData.direction; }
	const QVector3D& getPosition() { return m_lightData.position; }
	void setPosition(const QVector3D& pos) { m_lightData.position = pos; }
	const float setAttenuationConstant(const float val) { m_lightData.attenuationConstant = val; }
	float getAttenuationConstant() { return m_lightData.attenuationConstant; }
	const float setAttenuationLinear(const float val) { m_lightData.attenuationLinear = val; }
	float getAttenuationLinear() { return m_lightData.attenuationLinear; }
	const float setAttenuationQuadratic(const float val) { m_lightData.attenuationQuadratic = val; }
	float getAttenuationQuadratic() { return m_lightData.attenuationQuadratic; }

private:
	void init();

	aiLight* m_refLight = nullptr;
	LightData m_lightData;

};

