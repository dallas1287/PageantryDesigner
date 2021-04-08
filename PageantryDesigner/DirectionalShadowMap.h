#pragma once
#include "ShadowMap.h"

static const char* DIR_SHADOW_MAP_VS = "shadowMap_vs.glsl";
static const char* DIR_SHADOW_MAP_FRAG = "shadowMap_frag.glsl";

class DirectionalShadowMap : public ShadowMap
{
public:
	DirectionalShadowMap();
	~DirectionalShadowMap();

	virtual void initDepthMap() override;
	virtual void initFrameBuffer() override;

	void setLightSpaceMatrix(const QVector3D& lightPos);
	const QMatrix4x4& getLightSpaceMatrix() { return m_lightSpaceMatrix; }
	void setShaderLightSpaceMatrix();

private:
	QMatrix4x4 m_projection;
	QMatrix4x4 m_view;
	QMatrix4x4 m_lightSpaceMatrix;
};

