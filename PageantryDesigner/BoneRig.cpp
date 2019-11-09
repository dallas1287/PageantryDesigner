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

void BoneRig::getAllChildren(const QString& bone, std::vector<aiNode*>& children)
{
	aiNode* boneNode = findBoneInSkeleton(bone);
	if (boneNode)
	{
		children.push_back(boneNode);
		getAllChildrenRecursively(boneNode, children);
	}
}

void BoneRig::getAllChildrenRecursively(aiNode* boneNode, std::vector<aiNode*>& children)
{
	for (int i = 0; i < boneNode->mNumChildren; ++i)
	{
		QString name(boneNode->mChildren[i]->mName.data);
		getAllChildrenRecursively(boneNode->mChildren[i], children);
		children.push_back(boneNode->mChildren[i]);
	}
}