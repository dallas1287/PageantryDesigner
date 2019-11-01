#pragma once
#include "Includes/assimp/Importer.hpp"
#include "Includes/assimp/scene.h"
#include "Includes/assimp/postprocess.h"
#include "GraphicsObject.h"
#include "ShapeCreator.h"
#include "Bone.h"
#include "BoneRig.h"
#include "Animation.h"
#include <unordered_map>
#include "ItemRenderer.h"
#include "SceneNode.h"

class MeshObject : public GraphicsObject
{
public:
	MeshObject(aiMesh* ref);
	~MeshObject();
	void initialize();
	void initializeBuffers();
	aiMesh* getMeshRef() { return m_meshRef; }
	std::vector<VertexData>& getVertexData() { return m_meshData; }
	std::vector<GLushort>& getIndices() { return m_indices; }
	std::vector<Bone*>& DeformBones() { return m_deformBones; }
	
	Bone* findDeformBone(const QString& name);
	std::vector<BoneData>& getBoneData() { return m_boneData; }
	void createBoneData();
	void buildVertexTransforms();
	void moveDirectly();

	std::vector<VertexData> m_copyData;
private:
	void createBones();

	std::vector<VertexData> m_meshData;
	std::vector<GLushort> m_indices;
	std::vector<Bone*> m_deformBones;
	std::vector<BoneData> m_boneData;
	int numIndices = 0;
	aiMesh* m_meshRef = nullptr;
};

typedef std::vector<MeshObject*> MeshObjectPool;
typedef std::map<QString, SceneNode*> NodeMap;

class MeshManager
{
public:
	MeshManager(ItemRenderer* parent): m_parent(parent), m_boneRig(BoneRig(this)) {};
	~MeshManager()
	{
		for (auto meshObj : m_meshPool)
			delete meshObj;
		for (auto node : m_nodeMap)
			delete node.second;
	}
	bool import(const QString& path);
	BoneRig& getBoneRig() { return m_boneRig; }
	MeshObjectPool& getMeshes() { return m_meshPool; }
	QMatrix4x4& GlobalTransform() { return m_globalTransform; }
	QMatrix4x4 GlobalInverseTransform() { return m_globalTransform.inverted(); }
	SceneNode* findSceneNode(const QString& name);
	void animate();
	void incrementFrame();
	void decrementFrame();
	int getFrameCt() { return m_frameCt; }

private:
	void createMeshes(const aiScene* scene);
	void createSkeleton(aiNode* root);
	void createSceneNodes(aiNode* root);
	void createSceneNodesRecursively(aiNode* node);
	void createSceneTreeRecursively(aiNode* node);
	void animateRecursively(aiNode* node, const QMatrix4x4& parentTransform);
	QMatrix4x4 traverseTransforms(const QString& name);

	void LogAnimationData();

	ItemRenderer* m_parent;
	Assimp::Importer m_importer;
	QMatrix4x4 m_globalTransform;
	BoneRig m_boneRig;
	NodeMap m_nodeMap;
	MeshObjectPool m_meshPool;
	std::vector<Animation> m_animations;
	int m_frameCt = 0;
	bool logStuff = true;
};

