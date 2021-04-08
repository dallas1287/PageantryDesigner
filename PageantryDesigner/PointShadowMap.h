#pragma once
#include "ShadowMap.h"

static const char* POINT_SHADOW_MAP_VS = "shadowCubeMap_vs.glsl";
static const char* POINT_SHADOW_MAP_FRAG = "shadowCubeMap_frag.glsl";
static const char* POINT_SHADOW_MAP_GS = "shadowCubeMap_gs.glsl";

static const std::vector<std::pair<QVector3D, QVector3D>> SHADOW_TRANSFORM_MAP =
{
	{		 X,	-1.0 * Y},
	{ -1.0 * X, -1.0 * Y},
	{		 Y,		   Z},
	{ -1.0 * Y, -1.0 * Z},
	{		 Z, -1.0 * Y},
	{ -1.0 * Z, -1.0 * Y}
};

class PointShadowMap : public ShadowMap
{
public:
	PointShadowMap();
	~PointShadowMap();
	virtual void initDepthMap() override;
	virtual void initFrameBuffer() override;

	void setShaderShadowTransforms();
	void setShaderFarPlane();
	void setShaderLightPosition(const QVector3D& lightPos);

private:
	void initShadowTransforms(const QVector3D& lightPos = QVector3D());

	std::vector<QMatrix4x4> m_shadowTransforms;
};

