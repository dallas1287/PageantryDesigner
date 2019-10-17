#pragma once
#include "Includes/assimp/Importer.hpp"
#include "Includes/assimp/scene.h"
#include "Includes/assimp/postprocess.h"
#include "GraphicsObject.h"
#include "ShapeCreator.h"

class BoneRig
{
public:
	BoneRig() {};
	~BoneRig() {};
	bool buildBones(aiNode* rootBone)
	{
		if (!rootBone)
			return false;

		buildBonesRecursively(rootBone);

		qDebug() << "Map Size: " << m_bonesMap.size();

		m_rootBone = rootBone;
		return true;
	}

private:
	void buildBonesRecursively(aiNode* bone)
	{
		for (int i = 0; i < bone->mNumChildren; ++i)
		{
			buildBonesRecursively(bone->mChildren[i]);
			m_bonesMap[QString(bone->mChildren[i]->mName.C_Str())] = bone->mChildren[i];
		}
	}

	aiNode* m_rootBone = nullptr;
	std::map<QString, aiNode*> m_bonesMap;
};


class MeshObject : public GraphicsObject
{
public:
	MeshObject(aiMesh* ref);
	~MeshObject();
	aiMesh* getMeshRef() { return m_meshRef; }
	std::vector<VertexData>& getData() { return m_meshData; }
	std::vector<GLushort>& getIndices() { return m_indices; }
	std::vector<aiBone*>& Bones() { return m_bones; }
	BoneRig getBoneRig() { return m_boneRig; }
	aiBone* findBone(const QString& name);
	void initialize();
	void initializeBuffers();

private:
	std::vector<VertexData> m_meshData;
	std::vector<GLushort> m_indices;
	std::vector<aiBone*> m_bones;
	BoneRig m_boneRig;
	int numIndices = 0;
	aiMesh* m_meshRef = nullptr;
};

typedef std::vector<MeshObject*> MeshObjectPool;

class MeshManager
{
public:
	MeshManager();
	~MeshManager();
	bool import(const QString& path);
	MeshObjectPool& getMeshes() { return m_meshPool; }

private:
	Assimp::Importer m_importer;
	MeshObjectPool m_meshPool;
};

