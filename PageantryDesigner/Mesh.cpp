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

static bool unwritten = true;
void MeshObject::moveDirectly()
{
	for (auto bd : m_boneData)
	{
		m_meshData[bd.ID].position = (*bd.FinalTransform) * m_copyData[bd.ID].position;
		/*if (m_copyData[bd.ID].position.z() > 2.0 && unwritten)
		{
			std::ofstream outfile("../data.txt", std::ios_base::app);
			outfile << "Index: " << bd.ID << std::endl;
			outfile << "Original Position: " << vectorToStdString(m_copyData[bd.ID].position) << std::endl;
			outfile << "New Position: " << vectorToStdString(m_meshData[bd.ID].position) << std::endl;
			outfile << bd.boneWeights.begin()->first->getName().toLocal8Bit().constData() <<  " Offset: " << std::endl << matrixToStdString(bd.boneWeights.begin()->first->OffsetMatrix());
			outfile << "Transform Matrix: " << std::endl << matrixToStdString((*bd.FinalTransform)) << std::endl;
		}*/
	}
	unwritten = false;
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
	createSceneNodes(scene->mRootNode);
	createSkeleton(scene->mRootNode);
	m_globalTransform = convertTransformMatrix(scene->mRootNode->mTransformation);

	if (scene->HasAnimations())
	{
		for (int i = 0; i < scene->mNumAnimations; ++i)
			m_animations.emplace_back(Animation(scene->mAnimations[i]));
	}

	createMeshes(scene);

	//LogAnimationData();

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

		meshObj->m_copyData.assign(meshObj->getVertexData().begin(), meshObj->getVertexData().end());

		meshObj->createBoneData();

		meshObj->buildVertexTransforms();

		//for (int i = 0; i < meshObj->getVertexData().size(); ++i)
		//	meshObj->getVertexData()[i].position = m_nodeMap.find("Armature")->second->getTransformMatrix() * meshObj->m_copyData[i].position;
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
	meshObj->moveDirectly();

	m_frameCt++;
#if USE_COLLADA
	if (m_frameCt >= 24)
		m_frameCt = 0;
#else
	if (m_frameCt > anim.getDuration())
		m_frameCt = 0;
#endif
}


void MeshManager::animateRecursively(aiNode* node, const QMatrix4x4& parentTransform)
{
	QMatrix4x4 globalTransform, animTransform;
	AnimationNode animNode;

	if (m_animations[0].findAnimationNode(QString(node->mName.C_Str()), animNode))
	{
#if !USE_COLLADA
		QMatrix4x4 animTransform;
		if (!animNode.getClosestTransform(m_frameCt, animTransform))
			return;
#else
		animTransform = animNode.getTransformKeys()[m_frameCt].matrix;
#endif
		globalTransform = parentTransform * animTransform;
		Bone* childBone = m_meshPool[0]->findDeformBone(QString(node->mName.C_Str()));
		if (childBone)
		{
			childBone->setModified();
			QMatrix4x4 inv = GlobalInverseTransform();
			QMatrix4x4 offset = childBone->OffsetMatrix();
			QMatrix4x4 mul = inv * offset;
			QMatrix4x4 huh = inv * globalTransform;
			QMatrix4x4 final = inv * globalTransform * offset;
			QMatrix4x4 trav = traverseTransforms(childBone->getName());
			QMatrix4x4 exp =  offset.inverted()*trav;
			childBone->setTransform(GlobalInverseTransform() * globalTransform * childBone->OffsetMatrix());
		}
		/*if (logStuff)
		{
			QString fileName = "../" + animNode.getName() + "_" + QString::number(m_frameCt) + ".txt";
			std::ofstream outfile(fileName.toLocal8Bit().constData(), std::ios_base::app);
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

			if (childBone)
			{
				outfile << "Offset Matrix:" << std::endl;
				outfile << matrixToStdString(childBone->OffsetMatrix()) << std::endl;
			}
			else
				outfile << "No offset matrix, not a deform bone";

			if (childBone)
			{
				outfile << "Final Transform:" << std::endl;
				outfile << matrixToStdString(GlobalInverseTransform() * globalTransform * childBone->OffsetMatrix()) << std::endl;
				QVector4D point(1, 1, 5, 0);
				QMatrix4x4 rotate;
				rotate.rotate(90, X);
				outfile << "Rotation 90X:" << std::endl;
				outfile << matrixToStdString(rotate) << std::endl;
				QVector4D finalpt = rotate * GlobalInverseTransform() * globalTransform * childBone->OffsetMatrix() * point;
				outfile << "Final Rotated Transform:" << std::endl;
				outfile << matrixToStdString(rotate * GlobalInverseTransform() * globalTransform * childBone->OffsetMatrix()) << std::endl;
				outfile << "(1,1,5) rotated 90 X:" << std::endl;
				outfile << vectorToStdString(QVector3D(finalpt.x(), finalpt.y(), finalpt.z()));
			}
			outfile.close();
		}*/
	}
	for(int i = 0; i < node->mNumChildren; ++i)
		animateRecursively(node->mChildren[i], globalTransform);
}

void MeshManager::incrementFrame()
{
	m_frameCt++;
#if USE_COLLADA
	if (m_frameCt > 24)
		m_frameCt = 0;
#else
	if (m_frameCt > m_animations[0].getDuration())
		m_frameCt = 0;
#endif
	logStuff = true;
}

void MeshManager::decrementFrame()
{
	m_frameCt--;
#if USE_COLLADA
	if (m_frameCt < 0)
		m_frameCt = 24;
#else
	if (m_frameCt < 0)
		m_frameCt = m_animations[0].getDuration();
#endif
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
			outfile << "T Matrix: " << std::endl << matrixToStdString(node.getTransformKeys()[j].matrix) << std::endl;
		}
	}
	outfile.close();
}

void MeshManager::createSceneNodes(aiNode* root)
{
	createSceneNodesRecursively(root);
	createSceneTreeRecursively(root);
}

void MeshManager::createSceneNodesRecursively(aiNode* node)
{
	SceneNode* sceneNode = new SceneNode(node);
	for (int i = 0; i < node->mNumChildren; ++i)
		createSceneNodesRecursively(node->mChildren[i]);
	m_nodeMap[sceneNode->getName()] = sceneNode;
}

void MeshManager::createSceneTreeRecursively(aiNode* node)
{
	auto nodeIter = m_nodeMap.find(node->mName.C_Str());
	if (nodeIter != m_nodeMap.end())
	{
		SceneNode* sceneNode = nodeIter->second;
		if (node->mParent)
		{
			auto parentIter = m_nodeMap.find(node->mParent->mName.C_Str());
			if (parentIter != m_nodeMap.end())
			{
				sceneNode->setParent(parentIter->second);
				//if parent is the root node then the node is most likely scale 100 times too big
				if (!sceneNode->getParent()->HasParent())
					sceneNode->adjustScaling(.01);
			}
		}
		else
			sceneNode->setParent(nullptr);

		for (int a = 0; a < node->mNumChildren; ++a)
		{
			aiNode* child = node->mChildren[a];
			auto childIter = m_nodeMap.find(child->mName.C_Str());
			if (childIter != m_nodeMap.end())
				sceneNode->Children().push_back(childIter->second);
		}
	}

	for (int i = 0; i < node->mNumChildren; ++i)
		createSceneTreeRecursively(node->mChildren[i]);
}

SceneNode* MeshManager::findSceneNode(const QString& name)
{
	auto nodeIter = m_nodeMap.find(name);
	return nodeIter != m_nodeMap.end() ? nodeIter->second : nullptr;
}

QMatrix4x4 MeshManager::traverseTransforms(const QString& name)
{
	SceneNode* start = findSceneNode(name);
	QMatrix4x4 tmat = start->getTransformMatrix();

	SceneNode* parent = start->getParent();
	while (parent)
	{
		tmat = parent->getTransformMatrix() * tmat;
		parent = parent->getParent();
	}
	return tmat;
}