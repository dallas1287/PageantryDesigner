#pragma once
#include "Includes/assimp/scene.h"
#include <QString>
#include <QMatrix4x4>
#include <unordered_map>
#include <vector>
#include "Bone.h"
#include "SceneNode.h"

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
	void setSceneRoot(aiNode* root) { m_sceneRoot = root; }
	aiNode* getRootNode() { return m_sceneRoot; }
	void setRigRoot(SceneNode* rigRoot) { m_rigRoot = rigRoot; }
	SceneNode* getRigRoot() { return m_rigRoot; }
	aiNode* findBoneInSkeleton(const QString& bone);

private:
	void buildSkeletonRecursively(aiNode* bone);

	MeshManager* m_parent = nullptr;
	aiNode* m_sceneRoot = nullptr;
	SceneNode* m_rigRoot = nullptr;
	SkeletonMap m_skeletonMap;
};
