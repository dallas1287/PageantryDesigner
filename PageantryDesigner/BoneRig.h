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
class MeshObject;

struct BoneData
{
	std::unordered_map<Bone*, float> boneWeights;
	QMatrix4x4* FinalTransform;
	unsigned int ID;

	BoneData() : ID(-1) {}
	~BoneData() {}

	void resetFinalTransform() { (*FinalTransform).setToIdentity(); }
	void transformFromBones()
	{
		(*FinalTransform).setToIdentity();
		for (auto bone : boneWeights)
		{
			if (bone.first->isModified())
				(*FinalTransform) += (bone.first->findVertexWeight(ID)* bone.first->getTransform());
		}
	}
};

class BoneRig
{
public:
	BoneRig(MeshObject* parent) : m_parent(parent) {}
	~BoneRig() {}
	SkeletonMap& getSkeletonMap() { return m_skeletonMap; }
	bool buildSkeleton(aiNode* rootBone);
	void setSceneRoot(aiNode* sceneRoot) { m_sceneRoot = sceneRoot; }
	void buildVertexTransforms();
	void moveBone(const QString& bone, const QVector3D& location);
	void getAllChildren(const QString& bone, std::vector<aiNode*>& children);
	void getAllParents(const QString& bone, std::vector<aiNode*>& parents);
	QMatrix4x4 getGlobalTransformation(std::vector<aiNode*>& parents);
	std::vector<BoneData>& getBoneData() { return m_boneData; }

	void MeshTransformTest();

private:
	void buildSkeletonRecursively(aiNode* bone);
	void getAllChildrenRecursively(aiNode* boneNode, std::vector<aiNode*>& children);

	MeshObject* m_parent = nullptr;
	aiNode* m_rootBone = nullptr;
	aiNode* m_sceneRoot = nullptr;
	std::vector<BoneData> m_boneData;
	SkeletonMap m_skeletonMap;

	void getTransformDebug(aiNode* parent);

};
