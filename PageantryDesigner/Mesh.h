#pragma once
#include "Includes/assimp/Importer.hpp"
#include "Includes/assimp/scene.h"
#include "Includes/assimp/postprocess.h"
#include "GraphicsObject.h"
#include "ShapeCreator.h"

class MeshObject : public GraphicsObject
{
public:
	MeshObject(aiMesh* ref);
	~MeshObject();
	aiMesh* getMeshRef() { return m_meshRef; }
	std::vector<VertexData>& getData() { return m_meshData; }
	std::vector<GLushort>& getIndices() { return m_indices; }
	void initialize();
	void initializeBuffers();

private:
	std::vector<VertexData> m_meshData;
	std::vector<GLushort> m_indices;
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
	MeshObjectPool& getMeshes() { return m_meshes; }

private:
	Assimp::Importer m_importer;
	MeshObjectPool m_meshes;
};

