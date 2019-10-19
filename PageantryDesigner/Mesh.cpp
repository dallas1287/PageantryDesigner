#include "Mesh.h"
#include "GraphicsPanel.h"

MeshObject::MeshObject(aiMesh* ref) : GraphicsObject(), m_meshRef(ref), m_boneRig(BoneRig(this))
{
}

MeshObject::~MeshObject()
{
	for (auto bone : m_deformBones)
		delete bone;
	m_meshRef = nullptr;
}

void MeshObject::initialize()
{
	initShaders("mesh_vertex.glsl", "mesh_fragment.glsl");
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
	ShaderProgram()->setAttributeBuffer(TextureAttr(), GL_FLOAT, offset, 2, sizeof(VertexData));

	offset += sizeof(decltype(m_meshData[0].texCoord));

	ShaderProgram()->enableAttributeArray(NormAttr());
	ShaderProgram()->setAttributeBuffer(NormAttr(), GL_FLOAT, offset, 3, sizeof(VertexData));

	offset += sizeof(decltype(m_meshData[0].normal));

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

Bone* MeshObject::findDeformBone(const QString& name)
{
	auto boneIter = std::find_if(m_deformBones.begin(), m_deformBones.end(), [&](auto bone) {return bone->getName() == name; });
	if (boneIter != m_deformBones.end())
		return *boneIter;
	return nullptr;
}

BoneData MeshObject::createBoneData(int id)
{
	BoneData bd; 
	//find the bones that control the corresponding vertexId
	auto bonesRange = VBMap().equal_range(id);
	if (bonesRange.first != VBMap().end())
	{
		auto boneIter = bonesRange.first;
		while (boneIter != bonesRange.second)
		{
			//find all the bones and their weights
			Bone* defBone = findDeformBone(boneIter->second);
			if (defBone)
				bd.boneWeights[defBone] = defBone->findVertexWeight(id);
			++boneIter;
		}
	}
	bd.ID = id;
	return bd;
}

void MeshObject::moveBone(const QString& name, const QVector3D& location)
{
	m_boneRig.moveBone(name, location);
}

bool MeshManager::import(const QString& path)
{
	std::string pathstd = path.toStdString();
	const char* pathStr = pathstd.c_str();

	const aiScene* scene = m_importer.ReadFile(pathStr,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!scene)
	{
		qDebug() << m_importer.GetErrorString();
		return false;
	}

	//add the meshes
	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		m_meshPool.push_back(new MeshObject(scene->mMeshes[i]));
		MeshObject* meshObj = m_meshPool[i];
		aiMesh* mesh = meshObj->getMeshRef();
		VertexData point;
		QVector3D pos, norm;
		QVector2D tex;

		//add vertex data
		for (int j = 0; j < mesh->mNumVertices; ++j)
		{
			pos = QVector3D(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
			norm = QVector3D(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
			//not sure how this works as 3D coords but as far as I've seen the models I use don't have texture coordinates in the vertex data
			if (mesh->HasTextureCoords(0))
				tex = QVector2D(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
			point = VertexData(pos, tex, norm);
			meshObj->getVertexData().push_back(point);
		}

		for (int k = 0; k < mesh->mNumFaces; ++k)
		{
			const aiFace& face = mesh->mFaces[k];
			if (face.mNumIndices != 3)
			{
				//TODO: handle this error more completely
				qDebug() << "ERROR: Face NOT A TRIANGLE";
				return false;
			}
			for(int index = 0; index < 3; ++index)
				meshObj->getIndices().push_back(face.mIndices[index]);
		}

		//create the deform bones vector
		//recreates new custom objects from the assimp library objects
		if (mesh->HasBones())
		{
			for (int i = 0; i < mesh->mNumBones; ++i)
			{
				Bone* bone = new Bone();
				bone->initBone(mesh->mBones[i]);
				meshObj->DeformBones().push_back(bone);
			}
		}

		//create the deform bones map of vertices
		for (auto bone : meshObj->DeformBones())
		{
			for (int i = 0; i < bone->WeightCount(); ++i)
				meshObj->VBMap().insert({ bone->VertexWeights()[i].vertexID, bone->getName() });
		}

		//create the full bone rig hierarchy
		//aiNode* rig = scene->mRootNode->FindNode("rig");
		aiNode* rig = scene->mRootNode->FindNode("Armature");
		if (rig)
		{
			meshObj->getBoneRig().setSceneRoot(scene->mRootNode);
			meshObj->getBoneRig().buildSkeleton(rig);
		}
		//create Bone to Vertex data for each vertex
		for (int i = 0; i < meshObj->getMeshRef()->mNumVertices; ++i)
		{
			BoneData bd = meshObj->createBoneData(i);
			meshObj->getBoneRig().getBoneData().push_back(bd);
		}

		meshObj->getBoneRig().buildVertexTransforms();
	}
	return true;
}