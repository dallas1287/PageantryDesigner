#include "Mesh.h"
#include "GraphicsPanel.h"
#include <fstream>
#include "utils.h"

MeshObject::MeshObject(aiMesh* ref) : GraphicsObject(), m_meshRef(ref)
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

void MeshManager::traverseNodes(aiNode* node)
{
	qDebug() << node->mName.C_Str();
	qDebug() << matrixToString(convertTransformMatrix(node->mTransformation));
	aiVector3D scale, position;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, position);
	qDebug() << "Position: " << vectorToString(position);
	qDebug() << "Rotation: " << quaternionToString(rotate);
	qDebug() << "Scale: " << vectorToString(scale);

	for (int i = 0; i < node->mNumChildren; ++i)
		traverseNodes(node->mChildren[i]);
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

	createSkeleton(scene->mRootNode);
	m_globalTransform = convertTransformMatrix(scene->mRootNode->mTransformation);

	if (scene->HasAnimations())
	{
		for (int i = 0; i < scene->mNumAnimations; ++i)
			m_animations.emplace_back(Animation(scene->mAnimations[i]));
	}

	createMeshes(scene);

	LogAnimationData();

	return true;
}

void MeshManager::createMeshes(const aiScene* scene)
{
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
				//TODO: handle this error more completely, send a pop up dialog in gui
				qDebug() << "ERROR: Face NOT A TRIANGLE";
				return;
			}
			for (int index = 0; index < 3; ++index)
				meshObj->getIndices().push_back(face.mIndices[index]);
		}

		meshObj->createBoneData();

		meshObj->buildVertexTransforms();
	}
}

//create the full bone rig hierarchy
void MeshManager::createSkeleton(aiNode* root)
{
	//aiNode* rig = scene->mRootNode->FindNode("rig");
	aiNode* rig = root->FindNode("Armature");
	if (rig)
	{
		m_boneRig.setRootNode(root);
		m_boneRig.buildSkeleton(rig);
	}
}

//1. for each bone in the skeleton 
//2. Find corresponding animation node
//3. Get frame transform
//4. Finaltransform = GlobalInverse * parenttransform * node transform
void MeshManager::animate()
{
	Animation anim = m_animations[0];
	MeshObject* meshObj = getMeshes()[0];

	animateRecursively(getBoneRig().getRootNode(), QMatrix4x4());
	logStuff = false;
	for (auto bd : meshObj->getBoneData())
		bd.transformFromBones();

	m_frameCt++;
	if (m_frameCt > anim.getDuration())
		m_frameCt = 0;
}


void MeshManager::animateRecursively(aiNode* node, const QMatrix4x4& parentTransform)
{
	AnimationNode animNode;
	if (!m_animations[0].findAnimationNode(QString(node->mName.C_Str()), animNode))
		return;
	QMatrix4x4 animTransform = m_frameCt < animNode.getTransforms().size() ? animNode.getTransforms()[m_frameCt] : animNode.getTransforms().back();
	QMatrix4x4 globalTransform =  parentTransform * animTransform;
	Bone* childBone = m_meshPool[0]->findDeformBone(QString(node->mName.C_Str()));
	if (childBone)
	{
		childBone->setModified();
		childBone->setTransform(GlobalInverseTransform() * globalTransform * childBone->OffsetMatrix().transposed());
	}
	/*if (logStuff)
	{
		QString fileName = "../" + animNode.getName() + "_" + QString::number(m_frameCt) + ".txt";
		std::ofstream outfile(fileName.toLocal8Bit().constData(), std::ios_base::app);
		//outfile << LogCamera(m_parent->getParent()->getCamera());
		outfile << "Frame: " << m_frameCt << std::endl;
		outfile << animNode.getName().toLocal8Bit().constData() << std::endl;
		outfile << "Global Inverse: " << std::endl;
		outfile << matrixToStdString(GlobalInverseTransform());
		outfile << "Parent Transform: " << std::endl;
		outfile << matrixToStdString(parentTransform);
		outfile << "anim Transform:" << std::endl;
		outfile << matrixToStdString(animTransform);
		outfile << "globalTransform:(parent * anim)" << std::endl;
		outfile << matrixToStdString(globalTransform);
		outfile << "Offset Matrix:" << std::endl;
		if (childBone)
			outfile << matrixToStdString(childBone->OffsetMatrix()) << std::endl;
		else
			outfile << "No offset matrix, not a deform bone";
		outfile << "Final Transform:" << std::endl;
		if(childBone)
			outfile << matrixToStdString(GlobalInverseTransform() * globalTransform * childBone->OffsetMatrix()) << std::endl;
		outfile.close();
	}*/

	for(int i = 0; i < node->mNumChildren; ++i)
		animateRecursively(node->mChildren[i], globalTransform);
}

void MeshManager::incrementFrame()
{
	m_frameCt++;
	if (m_frameCt > m_animations[0].getDuration())
		m_frameCt = 0;

	logStuff = true;
}

void MeshManager::LogAnimationData()
{
	std::ofstream outfile("../AnimationData.txt");

	for (int i = 0; i < m_animations[0].getAnimNodes().size(); ++i)
	{
		AnimationNode node = m_animations[0].getAnimNodes()[i];
		outfile << node.getName().toLocal8Bit().constData() << std::endl;
		for (int j = 0; j < node.getPositionKeys().size(); ++j)
		{
			outfile << "\t\tFrame: " << j << std::endl;

			outfile << "P Vector: " << vectorToStdString(node.getPositionKeys()[j].value) << std::endl;
			outfile << "P Matrix: " << std::endl << matrixToStdString(node.getPositionKeys()[j].matrix) << std::endl;
			outfile << "R Quaternion: " << quaternionToStdString(node.getRotationKeys()[j].value) << std::endl;
			outfile << "R Matrix: " << std::endl << matrixToStdString(node.getRotationKeys()[j].matrix) << std::endl;
			outfile << "S Vector: " << vectorToStdString(node.getScalingKeys()[j].value) << std::endl;
			outfile << "S Matrix: " << std::endl << matrixToStdString(node.getScalingKeys()[j].matrix) << std::endl;
		}
	}
	outfile.close();
}