#include "MeshObject.h"


MeshObject::MeshObject(aiMesh* ref) : GraphicsObject(), m_meshRef(ref), m_name(ref->mName.data)
{
	createBones();
}

MeshObject::~MeshObject()
{
	for (auto bone : m_deformBones)
		delete bone;
	m_meshRef = nullptr;
}

void MeshObject::initialize()
{
	initializeOpenGLFunctions();
	initShaders("mesh_vertex.glsl", "color_frag.glsl");
	initializeBuffers();
}

void MeshObject::initializeBuffers()
{
	bindAll();
	Vbo().allocate(&m_meshData[0], m_meshData.size() * sizeof(VertexData));

	int offset = 0;

	ShaderProgram()->enableAttributeArray(PosAttr());
	ShaderProgram()->setAttributeBuffer(PosAttr(), GL_FLOAT, offset, 3, sizeof(VertexData));

	offset += sizeof(decltype(m_meshData[0].position));

	ShaderProgram()->enableAttributeArray(TextureAttr());
	ShaderProgram()->setAttributeBuffer(TextureAttr(), GL_FLOAT, offset, 3, sizeof(VertexData));

	offset += sizeof(decltype(m_meshData[0].texCoord));

	ShaderProgram()->enableAttributeArray(NormAttr());
	ShaderProgram()->setAttributeBuffer(NormAttr(), GL_FLOAT, offset, 3, sizeof(VertexData));

	offset += sizeof(decltype(m_meshData[0].normal));

	ShaderProgram()->enableAttributeArray(ColorAttr());
	ShaderProgram()->setAttributeBuffer(ColorAttr(), GL_FLOAT, offset, 4, sizeof(VertexData));

	offset += sizeof(decltype(m_meshData[0].color));

	ShaderProgram()->enableAttributeArray(BoneAttr0());
	ShaderProgram()->setAttributeBuffer(BoneAttr0(), GL_FLOAT, offset, 4, sizeof(VertexData));

	offset += sizeof(QVector4D);

	ShaderProgram()->enableAttributeArray(BoneAttr1());
	ShaderProgram()->setAttributeBuffer(BoneAttr1(), GL_FLOAT, offset, 4, sizeof(VertexData));

	offset += sizeof(QVector4D);

	ShaderProgram()->enableAttributeArray(BoneAttr2());
	ShaderProgram()->setAttributeBuffer(BoneAttr2(), GL_FLOAT, offset, 4, sizeof(VertexData));

	offset += sizeof(QVector4D);

	ShaderProgram()->enableAttributeArray(BoneAttr3());
	ShaderProgram()->setAttributeBuffer(BoneAttr3(), GL_FLOAT, offset, 4, sizeof(VertexData));

	Ebo().allocate(&m_indices[0], m_indices.size() * sizeof(GLushort));
	releaseAll();
}

void MeshObject::createBones()
{
	if (!m_meshRef)
		return;

	if (m_meshRef->HasBones())
	{
		for (int i = 0; i < m_meshRef->mNumBones; ++i)
			m_deformBones.emplace_back(new Bone(m_meshRef->mBones[i]));
	}
}

Bone* MeshObject::findDeformBone(const QString& name)
{
	auto boneIter = std::find_if(m_deformBones.begin(), m_deformBones.end(), [&](auto bone) {return bone->getName() == name; });
	if (boneIter != m_deformBones.end())
		return *boneIter;
	return nullptr;
}

void MeshObject::createBoneData()
{
	for (int id = 0; id < m_meshData.size(); ++id)
	{
		BoneData bd;
		bd.ID = id;
		for (auto bone : DeformBones())
		{
			auto bdIter = bone->getVertexWeightMap().find(id);
			if (bdIter != bone->getVertexWeightMap().end())
				bd.boneWeights[bone] = bdIter->second;
		}
		m_boneData.push_back(bd);
	}

}

void MeshObject::buildVertexTransforms()
{
	if (getVertexData().size() != m_boneData.size())
	{
		qDebug() << "MISMATCHED data containers!!!";
		return;
	}

	for (int i = 0; i < m_boneData.size(); ++i)
		m_boneData[i].FinalTransform = &getVertexData()[i].transform;
}
