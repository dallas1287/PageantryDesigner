#pragma once
#include "Includes/assimp/scene.h"
#include <QString>
#include <QMatrix4x4>
#include <unordered_map>
#include <vector>
#include "Bone.h"

typedef std::map<QString, aiNode*> SkeletonMap;

//forward declaration - weird as hell that i have to foward declare here
//and include Mesh.h in the cpp file and can't include it here without errors
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
	QMatrix4x4 getGlobalTransformation(std::vector<aiNode*>& parents);
	//void moveDirectly(const QString& bone);

private:
	void buildSkeletonRecursively(aiNode* bone);
	void getAllChildrenRecursively(aiNode* boneNode, std::vector<aiNode*>& children);

	MeshManager* m_parent = nullptr;
	aiNode* m_rootNode = nullptr;
	SkeletonMap m_skeletonMap;
};
