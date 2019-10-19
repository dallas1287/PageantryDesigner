#pragma once
#include "Includes/assimp/Importer.hpp"
#include "Includes/assimp/scene.h"
#include "Includes/assimp/postprocess.h"
#include "GraphicsObject.h"
#include "ShapeCreator.h"
#include "Bone.h"
#include "BoneRig.h"
#include <unordered_map>

//Key = VertexID - Value = boneName
typedef std::unordered_multimap<unsigned int, QString> VertexBoneMap;

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
	BoneRig& getBoneRig() { return m_boneRig; }
	VertexBoneMap& VBMap() { return m_vbMap; }
	
	Bone* findDeformBone(const QString& name);
	BoneData createBoneData(int id);
	void moveBone(const QString& bone, const QVector3D& location);

private:
	std::vector<VertexData> m_meshData;
	std::vector<GLushort> m_indices;
	std::vector<Bone*> m_deformBones;
	VertexBoneMap m_vbMap;
	BoneRig m_boneRig;
	int numIndices = 0;
	aiMesh* m_meshRef = nullptr;
};

typedef std::vector<MeshObject*> MeshObjectPool;

class MeshManager
{
public:
	MeshManager() {};
	~MeshManager()
	{
		for (auto meshObj : m_meshPool)
			delete meshObj;
	}
	bool import(const QString& path);
	MeshObjectPool& getMeshes() { return m_meshPool; }

private:
	Assimp::Importer m_importer;
	MeshObjectPool m_meshPool;
};

