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

class MeshManager
{
public:
	MeshManager(): m_boneRig(BoneRig(this)) {};
	~MeshManager()
	{
		for (auto meshObj : m_meshPool)
			delete meshObj;
	}
	bool import(const QString& path);
	BoneRig& getBoneRig() { return m_boneRig; }
	MeshObjectPool& getMeshes() { return m_meshPool; }
	QMatrix4x4& GlobalTransform() { return m_globalTransform; }
	QMatrix4x4& GlobalInverseTransform() { return m_globalTransform.inverted(); }
	void animate();

	void LogDebugTransforms();

private:
	void createMeshes(const aiScene* scene);
	void createSkeleton(aiNode* root);
	void animateRecursively(aiNode* node, const QMatrix4x4& parentTransform);

	Assimp::Importer m_importer;
	QMatrix4x4 m_globalTransform;
	BoneRig m_boneRig;
	MeshObjectPool m_meshPool;
	std::vector<Animation> m_animations;
	int m_frameCt = 0;
};

