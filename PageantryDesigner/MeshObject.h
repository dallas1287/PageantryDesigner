#pragma once
#include "Includes/assimp/scene.h"
#include "GraphicsObject.h"
#include "utils.h"
#include "Bone.h"
#include "common.h"

struct BoneData
{
	std::unordered_map<Bone*, float> boneWeights;
	QMatrix4x4* FinalTransform;
	unsigned int ID;

	BoneData() : ID(-1) {}
	~BoneData() {}

	void resetFinalTransform() { (*FinalTransform).setToIdentity(); }
	bool transformFromBones()
	{
		bool ismodified = false;
		(*FinalTransform).fill(0.0);
		for (auto bone : boneWeights)
		{
			if (bone.first->isModified())
			{
				(*FinalTransform) += (bone.first->findVertexWeight(ID) * bone.first->getTransform());
				ismodified = true;
			}
		}
		if (!ismodified)
			resetFinalTransform();
		return ismodified;
	}
};

class MeshObject : public GraphicsObject
{
public:
	MeshObject(aiMesh* ref);
	~MeshObject();
	void initialize();
	const QString& getName() { return m_name; }
	aiMesh* getMeshRef() { return m_meshRef; }
	std::vector<VertexData>& getVertexData() { return m_meshData; }
	std::vector<GLushort>& getIndices() { return m_indices; }
	std::vector<Bone*>& DeformBones() { return m_deformBones; }
	unsigned int getMaterialIndex() { return m_materialIndex; }
	void setMaterialIndex(int index) { m_materialIndex = index; }
	void setMeshColor(aiColor3D& color) { m_meshColor = gammaCorrected(QVector4D(color.r, color.g, color.b, 1.0)); }
	void setMeshColor(QVector4D& color) { m_meshColor = gammaCorrected(color); }
	QVector4D& getMeshColor() { return m_meshColor; }
	bool isRigged() { return m_isRigged; }
	void setIsRigged(bool rigged) { m_isRigged = rigged; }

	Bone* findDeformBone(const QString& name);
	std::vector<BoneData>& getBoneData() { return m_boneData; }
	void createBoneData();
	void buildVertexTransforms();

private:
	void createBones();

	QString m_name;
	std::vector<VertexData> m_meshData;
	std::vector<GLushort> m_indices;
	std::vector<Bone*> m_deformBones;
	std::vector<BoneData> m_boneData;
	unsigned int numIndices = 0;
	bool m_isRigged = false;
	unsigned int m_materialIndex = -1;
	QVector4D m_meshColor;
	aiMesh* m_meshRef = nullptr;
};
