#pragma once
#include "Includes/assimp/scene.h"
#include <QString>
#include <QMatrix4x4>
#include <unordered_map>
#include <vector>
#include "Bone.h"

typedef std::map<QString, aiNode*> SkeletonMap;

class MeshManager;
class MeshObject;

class BoneRig
{
public:
	BoneRig(MeshManager* parent) : m_parent(parent) {}
	~BoneRig() {}
	SkeletonMap& getSkeletonMap() { return m_skeletonMap; }
	bool buildSkeleton(aiNode* rootNode);
	void setRootNode(aiNode* root) { m_rootNode = root; }
	aiNode* getRootNode() { return m_rootNode; }

	//void moveBone(const QString& bone, const QVector3D& location);
	aiNode* findBoneInSkeleton(const QString& bone);
	void getAllChildren(const QString& bone, std::vector<aiNode*>& children);
	//void moveDirectly(const QString& bone);

private:
	void buildSkeletonRecursively(aiNode* bone);
	void getAllChildrenRecursively(aiNode* boneNode, std::vector<aiNode*>& children);

	MeshManager* m_parent = nullptr;
	aiNode* m_rootNode = nullptr;
	SkeletonMap m_skeletonMap;
};
