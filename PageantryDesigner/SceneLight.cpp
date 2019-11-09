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
	m_ambientColor = convertAiToVector(m_refLight->mColorAmbient);
	m_diffuseColor = convertAiToVector(m_refLight->mColorDiffuse);
	m_specularColor = convertAiToVector(m_refLight->mColorSpecular);
	m_direction = convertAiToVector(m_refLight->mDirection);
	m_position = convertAiToVector(m_refLight->mPosition);
	m_attenuationConstant = m_refLight->mAttenuationConstant;
	m_attenuationLinear = m_refLight->mAttenuationLinear;
	m_attenuationQuadratic = m_refLight->mAttenuationQuadratic;

}