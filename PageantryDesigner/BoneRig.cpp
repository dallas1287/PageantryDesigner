#include "BoneRig.h"
#include <QDebug>
#include "utils.h"

bool BoneRig::buildSkeleton(aiNode* rootNode)
{
	if (!rootNode)
		return false;

	buildSkeletonRecursively(rootNode);
	return true;
}

void BoneRig::buildSkeletonRecursively(aiNode* bone)
{
	for (int i = 0; i < bone->mNumChildren; ++i)
	{
		buildSkeletonRecursively(bone->mChildren[i]);
		//if it has meshes it's not a bone
		if (bone->mChildren[i]->mNumMeshes == 0)
			m_skeletonMap[QString(bone->mChildren[i]->mName.C_Str())] = bone->mChildren[i];
	}
}

aiNode* BoneRig::findBoneInSkeleton(const QString& bone)
{
	if (m_skeletonMap.find(bone) == m_skeletonMap.end())
		return nullptr;

	return m_skeletonMap[bone];
}