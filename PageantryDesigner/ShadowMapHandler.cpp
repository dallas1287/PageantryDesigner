#include "ShadowMapHandler.h"

ShadowMapHandler::ShadowMapHandler()
{
	m_dirShadowMap.reset(new DirectionalShadowMap());
	m_pointShadowMap.reset(new PointShadowMap());
}

ShadowMapHandler::~ShadowMapHandler()
{
}

void ShadowMapHandler::initFrameBuffer(int type)
{
	switch (type)
	{
	case ShadowType::Directional:
		m_dirShadowMap->initFrameBuffer();
		break;
	case ShadowType::Point:
		m_pointShadowMap->initFrameBuffer();
		break;
	default:
		break;
	}
}

void ShadowMapHandler::initShaders()
{
	m_dirShadowMap->initShaders(DIR_SHADOW_MAP_VS, DIR_SHADOW_MAP_FRAG);
	m_pointShadowMap->initShaders(POINT_SHADOW_MAP_VS, POINT_SHADOW_MAP_FRAG, POINT_SHADOW_MAP_GS);
}

void ShadowMapHandler::setCurrentSM(int type)
{
	switch (type)
	{
	case ShadowType::Directional:
		m_currentSM = m_dirShadowMap.get();
		break;
	case ShadowType::Point:
		m_currentSM = m_pointShadowMap.get();
		break;
	default:
		break;
	}
}