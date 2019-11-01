#include "SceneNode.h"
#include "utils.h"

SceneNode::SceneNode(aiNode* refNode): m_refNode(refNode)
{
	init();
}

SceneNode::~SceneNode()
{
}

void SceneNode::init()
{
	if (!m_refNode)
		return;

	m_name = m_refNode->mName.C_Str();
	m_transformMatrix = convertTransformMatrix(m_refNode->mTransformation);
}
