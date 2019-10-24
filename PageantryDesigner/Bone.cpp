#include "Bone.h"
#include "utils.h"

void Bone::initBone(aiBone* refBone)
{
	m_name = QString(refBone->mName.data);
	m_numWeights = refBone->mNumWeights;
	m_OffsetMatrix = convertTransformMatrix(refBone->mOffsetMatrix);
	for (int i = 0; i < refBone->mNumWeights; ++i)
		m_vertexWeightMap[refBone->mWeights[i].mVertexId] = refBone->mWeights[i].mWeight;
}

float Bone::findVertexWeight(unsigned int id)
{
	auto weight = m_vertexWeightMap.find(id);

	if (weight != m_vertexWeightMap.end())
		return weight->second;

	return 0.0;
}