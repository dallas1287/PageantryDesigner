#include "SceneLight.h"
#include "utils.h"

SceneLight::SceneLight(aiLight* refLight) : m_refLight(refLight)
{
	init();
}

void SceneLight::init()
{
	if (!m_refLight)
		return;

	m_name = QString(m_refLight->mName.C_Str());
	m_lightData.ambient = convertAiToVector(m_refLight->mColorAmbient);
	m_lightData.diffuse = convertAiToVector(m_refLight->mColorDiffuse);
	m_lightData.specular = convertAiToVector(m_refLight->mColorSpecular);
	m_lightData.direction = convertAiToVector(m_refLight->mDirection);
	m_lightData.position = convertAiToVector(m_refLight->mPosition);
	m_lightData.attenuationConstant = m_refLight->mAttenuationConstant;
	m_lightData.attenuationLinear = m_refLight->mAttenuationLinear;
	m_lightData.attenuationQuadratic = m_refLight->mAttenuationQuadratic;
}