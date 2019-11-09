#pragma once
#include "Includes/assimp/scene.h"
#include "GraphicsObject.h"
#include <QOpenGLExtraFunctions>
#include "utils.h"
#include "Bone.h"
#include "common.h"

class MeshObject : public GraphicsObject, protected QOpenGLExtraFunctions
{
public:
	MeshObject(aiMesh* ref);
	~MeshObject();
	void initialize();
	void initializeBuffers();
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
