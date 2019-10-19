#include "Bone.h"
#include "utils.h"

void Bone::initBone(aiBone* refBone)
{
	m_name = QString(refBone->mName.data);
	m_numWeights = refBone->mNumWeights;
	m_OffsetMatrix = convertTransformMatrix(refBone->mOffsetMatrix);
	for (int i = 0; i < refBone->mNumWeights; ++i)
	{
		VertexWeight vw(refBone->mWeights[i].mVertexId, refBone->mWeights[i].mWeight);
		m_vertexWeights.push_back(vw);
	}
}

float Bone::findVertexWeight(unsigned int id)
{
	auto vwIter = std::find_if(m_vertexWeights.begin(), m_vertexWeights.end(), [&](auto vw) {return vw.vertexID == id; });
	if (vwIter == m_vertexWeights.end())
		return 0.0;

	return (*vwIter).weight;
}