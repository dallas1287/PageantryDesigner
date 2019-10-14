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

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
	for (auto meshObj : m_meshes)
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

	//add the meshes
	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		m_meshes.push_back(new MeshObject(scene->mMeshes[i]));
	}
	//add the vertex data
	for (int i = 0; i < m_meshes.size(); ++i)
	{
		MeshObject* meshObj = m_meshes[i];
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
	}

	qDebug() << "NumMeshes: " << scene->mNumMeshes;
	qDebug() << "NumMaterials: " << scene->mNumMaterials;
	qDebug() << "Has Normals " << scene->mMeshes[0]->HasNormals();
	qDebug() << "Has Bones " << scene->mMeshes[0]->HasBones();
	qDebug() << "NumBones: " << scene->mMeshes[0]->mNumBones;
	qDebug() << "NumFaces: " << scene->mMeshes[0]->mNumFaces;
	qDebug() << "NumVerts: " << scene->mMeshes[0]->mNumVertices;
	qDebug() << "NumIndices" << scene->mMeshes[0]->mFaces[0].mNumIndices;

	return true;
}