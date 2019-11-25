#pragma once
#include "DirectionalShadowMap.h"
#include "PointShadowMap.h"

class ShadowMapHandler
{
public:
	ShadowMapHandler();
	~ShadowMapHandler();
	DirectionalShadowMap* DirectionalSM() { return m_dirShadowMap.get(); }
	PointShadowMap* PointSM() { return m_pointShadowMap.get(); }
	void initFrameBuffer(int type);
	void initShaders();
	ShadowMap* currentSM() { return m_currentSM; }
	void setCurrentSM(ShadowMap* current) { m_currentSM = current; }
	void setCurrentSM(int type);

private:
	std::unique_ptr<DirectionalShadowMap> m_dirShadowMap;
	std::unique_ptr<PointShadowMap> m_pointShadowMap;

	ShadowMap* m_currentSM = nullptr;
};

