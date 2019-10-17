#include "Mesh.h"
#include "GraphicsPanel.h"

MeshObject::MeshObject(aiMesh* ref) : GraphicsObject(), m_meshRef(ref)
{
}

MeshObject::~MeshObject()
{
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

	ShaderProgram()->enableAttributeArray(PosAttr());
	ShaderProgram()->setAttributeBuffer(PosAttr(), GL_FLOAT, 0, 3, sizeof(VertexData));

	ShaderProgram()->enableAttributeArray(TextureAttr());
	ShaderProgram()->setAttributeBuffer(TextureAttr(), GL_FLOAT, sizeof(decltype(m_meshData[0].position)), 2, sizeof(VertexData));

	ShaderProgram()->enableAttributeArray(NormAttr());
	ShaderProgram()->setAttributeBuffer(NormAttr(), GL_FLOAT, sizeof(decltype(m_meshData[0].position)) + sizeof(decltype(m_meshData[0].texCoord)), 3, sizeof(VertexData));

	Ebo().allocate(&m_indices[0], m_indices.size() * sizeof(GLushort));
	releaseAll();
}

aiBone* MeshObject::findBone(const QString& name)
{
	auto boneIter = std::find_if(m_bones.begin(), m_bones.end(), [&](auto bone) {return QString(bone->mName.C_Str()) == name; });
	if (boneIter != m_bones.end())
		return *boneIter;
	return nullptr;
}

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
	for (auto meshObj : m_meshPool)
		delete meshObj;
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

	aiNode* root = scene->mRootNode;
	if (root)
	{
		qDebug() << root->mTransformation.a1 << root->mTransformation.a2 << ", " << root->mTransformation.a3  << ", " << root->mTransformation.a4;
	}
	//add the meshes
	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		m_meshPool.push_back(new MeshObject(scene->mMeshes[i]));
	}
	//add the vertex data
	for (int i = 0; i < m_meshPool.size(); ++i)
	{
		MeshObject* meshObj = m_meshPool[i];
		aiMesh* mesh = meshObj->getMeshRef();
		VertexData point;
		QVector3D pos, norm;
		QVector2D tex;

		for (int j = 0; j < mesh->mNumVertices; ++j)
		{
			pos = QVector3D(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
			norm = QVector3D(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
			//not sure how this works as 3D coords but as far as I've seen the models I use don't have texture coordinates in the vertex data
			if (mesh->HasTextureCoords(0))
				tex = QVector2D(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
			point = VertexData(pos, tex, norm);
			meshObj->getData().push_back(point);
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

		if (mesh->HasBones())
		{
			for (int i = 0; i < mesh->mNumBones; ++i)
				meshObj->Bones().push_back(mesh->mBones[i]);
		}

		aiNode* rig = root->FindNode("rig");
		if (rig)
			meshObj->getBoneRig().buildBones(rig);
	}
	return true;
}