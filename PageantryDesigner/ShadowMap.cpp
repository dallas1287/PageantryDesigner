#include "ShadowMap.h"
#include "utils.h"

ShadowMap::ShadowMap() : GraphicsObject()
{
	initDepthQuad();
}

ShadowMap::~ShadowMap()
{
}

void ShadowMap::initDepthQuad()
{
	m_quad.reset(new GraphicsObject());
	VertexDataPool vdata;
	IndexPool idata;
	ShapeMaker::Quad::createQuad(vdata, idata);
	m_quad->setVertexData(vdata);
	m_quad->setIndices(idata);
	m_quad->initShaders("depthMap_vs.glsl", "depthMap_frag.glsl");
	m_quad->initBuffers();
}

void ShadowMap::setViewport()
{
	glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
}

void ShadowMap::setModelUniform(const QMatrix4x4& model)
{
	if (!ShaderProgram())
		return;
	ShaderProgram()->bind();
	ShaderProgram()->setUniformValue("model", model);
	ShaderProgram()->release();
}