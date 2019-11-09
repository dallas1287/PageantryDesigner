#include "Mesh.h"
#include "GraphicsPanel.h"
#include <fstream>

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

MeshManager::~MeshManager()
{
	for (auto meshObj : m_meshPool)
		delete meshObj;
	for (auto node : m_nodeMap)
		delete node.second;
	for (auto anim : m_animations)
		delete anim;
}

void MeshManager::resetData()
{
	m_globalTransform.setToIdentity();
	m_boneRig.reset();
	for (auto sceneNode : m_nodeMap)
		delete sceneNode.second;
	m_nodeMap.clear();
	for (auto mesh : m_meshPool)
		delete mesh;
	m_meshPool.clear();
	m_currentMesh = nullptr;
	for (auto anim : m_animations)
		delete anim;
	m_animations.clear();
	m_currentAnimation = nullptr;
	m_frameCt = 0;
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

	m_globalTransform = convertTransformMatrix(scene->mRootNode->mTransformation);

	if (scene->HasCameras())
	{
		for (int i = 0; i < scene->mNumCameras; ++i)
			qDebug() << "Camera: " << scene->mCameras[i]->mName.data;
	}

	if (scene->HasLights())
	{
		for (int i = 0; i < scene->mNumLights; ++i)
			qDebug() << "Light: " << scene->mLights[i]->mName.data;
	}

	createSceneNodes(scene->mRootNode);

	if (scene->HasAnimations())
	{
		for (int i = 0; i < scene->mNumAnimations; ++i)
			m_animations.emplace_back(new Animation(scene->mAnimations[i]));
		setCurrentAnimation(0);
	}

	createMeshes(scene);
	createSkeleton(scene);
	return true;
}

SceneNode* MeshManager::findArmatureNode(const aiScene* scene)
{
	if (!scene)
		return nullptr;
	std::vector<QString> names;

	for (int i = 0; i < scene->mRootNode->mNumChildren; ++i)
	{
		bool foundName = false;
		QString name(scene->mRootNode->mChildren[i]->mName.data);
		//not a mesh
		if (findMesh(name))
			foundName = true;

		if (!foundName && scene->HasLights())
		{
			for (int i = 0; i < scene->mNumLights; ++i)
			{
				QString lightName(scene->mLights[i]->mName.data);
				if (lightName.compare(name) == 0)
				{
					foundName = true;
					break;
				}
			}
		}

		if (!foundName && scene->HasCameras())
		{
			for (int i = 0; i < scene->mNumCameras; ++i)
			{
				QString camName(scene->mCameras[i]->mName.data);
				if (camName.compare(name) == 0)
				{
					foundName = true;
					break;
				}
			}
		}

		if (!foundName)
			names.push_back(name);
	}

	if (names.size() == 0)
		return nullptr;

	if (names.size() == 1)
		return findSceneNode(names.front());

	auto iter = names.begin();
	while(iter != names.end())
	{
		bool foundBone = false;
		for (auto mesh : getMeshes())
		{
			if (mesh->findDeformBone(*iter))
			{
				foundBone = true;
				continue;
			}
		}
		if (!foundBone)
			iter = names.erase(iter);
		else
			++iter;
	}

	return findSceneNode(names.front());
}

void MeshManager::createMeshes(const aiScene* scene)
{
	//add the meshes
	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		m_meshPool.emplace_back(new MeshObject(scene->mMeshes[i]));
		MeshObject* meshObj = m_meshPool.back();
		meshObj->setIsRigged(scene->mMeshes[i]->HasBones());
		aiMesh* mesh = meshObj->getMeshRef();
		VertexData point;
		QVector3D pos, norm;
		QVector3D tex;

		if (scene->mNumMaterials > mesh->mMaterialIndex)
		{
			meshObj->setMaterialIndex(mesh->mMaterialIndex);
			aiMaterial* material = scene->mMaterials[meshObj->getMaterialIndex()];
			if (material)
			{
				aiString name;
				if (material->Get(AI_MATKEY_NAME, name))
					qDebug() << name.data;
				aiColor3D diffuseColor(0.0, 0.0, 0.0);
				if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
					meshObj->setMeshColor(diffuseColor);
			}
		}

		if (scene->mNumMaterials)
		{
			for (int i = 0; i < scene->mNumMaterials; ++i)
			{
				aiMaterial* material = scene->mMaterials[i];
				aiString name;
				if (AI_SUCCESS == material->Get(AI_MATKEY_NAME, name))
					qDebug() << name.data;
				aiColor3D difColor(0.0, 0.0, 0.0);
				if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, difColor))
					qDebug() << "Has Color Diffuse";
				aiColor3D specColor(0.0, 0.0, 0.0);
				if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, specColor))
					qDebug() << "Has Specular Color";
				aiColor3D ambColor(0.0, 0.0, 0.0);
				if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, ambColor))
					qDebug() << "Has Ambient Color";
				aiColor3D emissiveColor(0.0, 0.0, 0.0);
				if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor))
					qDebug() << "Has Emissive Color";
				aiColor3D transColor(0.0, 0.0, 0.0);
				if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_TRANSPARENT, transColor))
					qDebug() << "Has Transparent Color";
				float shininess = 0.0;
				if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, shininess));
				float opacity = 0.0;
				if (AI_SUCCESS == material->Get(AI_MATKEY_OPACITY, opacity))
					qDebug() << opacity;
				aiString file;
				if (AI_SUCCESS == material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), file))
					qDebug() << "Holy shit found a texture";
				unsigned int type = -1;
				if (AI_SUCCESS == material->Get(AI_MATKEY_SHADING_MODEL, type))
					qDebug() << "Has shading model";
			}
		}

		//add vertex data
		for (int j = 0; j < mesh->mNumVertices; ++j)
		{
			pos = QVector3D(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
			norm = QVector3D(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
			//not sure how this works as 3D coords
			if (mesh->HasTextureCoords(0))
				tex = QVector3D(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y, mesh->mTextureCoords[0][j].z);
			point = VertexData(pos, tex, norm, meshObj->getMeshColor());
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

MeshObject* MeshManager::findMesh(const QString& name)
{
	auto meshIter = std::find_if(m_meshPool.begin(), m_meshPool.end(), [&](auto mesh) {return mesh->getName() == name; });
	if (meshIter != m_meshPool.end())
		return *meshIter;
	return nullptr;
}

//create the full bone rig hierarchy
void MeshManager::createSkeleton(const aiScene* scene)
{
	if (!scene)
		return;
	if (!m_boneRig)
		m_boneRig.reset(new BoneRig(this));

	SceneNode* sn = findArmatureNode(scene);

	aiNode* rig = scene->mRootNode->FindNode(sn->getName().toLocal8Bit().constData());
	if (rig)
	{
		m_boneRig->setRootNode(scene->mRootNode);
		m_boneRig->buildSkeleton(rig);
	}
}

//1. for each bone in the skeleton 
//2. Find corresponding animation node
//3. Get frame transform
//4. Finaltransform = GlobalInverse * parenttransform * node transform
void MeshManager::animate()
{
	if(!getCurrentAnimation())
		return;
	for (auto meshObj : getMeshes())
	{
		setCurrentMesh(meshObj);
		animateRecursively(getBoneRig()->getRootNode(), QMatrix4x4());
		for (auto bd : meshObj->getBoneData())
			bd.transformFromBones();
		meshObj->initializeBuffers();
	}

	if (!(FigureRenderer*)m_parent->getParent()->isPaused())
	{
		m_frameCt++;
#if USE_COLLADA
		if (m_frameCt >= 24)
			m_frameCt = 0;
#else
		if (m_frameCt > getCurrentAnimation()->getDuration())
			m_frameCt = 0;
#endif
	}
}


void MeshManager::animateRecursively(aiNode* node, const QMatrix4x4& parentTransform)
{
	if (!node)
		return;

	QMatrix4x4 globalTransform, animTransform;
	AnimationNode* animNode = getCurrentAnimation()->findAnimationNode(QString(node->mName.C_Str()));

	if (animNode)
	{
#if !USE_COLLADA
		QMatrix4x4 animTransform;
		if (!animNode->getClosestTransform(m_frameCt, animTransform))
			return;
#else
		animTransform = animNode.getTransformKeys()[m_frameCt].matrix;
#endif
		globalTransform = parentTransform * animTransform;
		Bone* childBone = getCurrentMesh()->findDeformBone(QString(node->mName.C_Str()));
		if (childBone)
		{
			childBone->setModified();
			childBone->setTransform(GlobalInverseTransform() * globalTransform * childBone->OffsetMatrix());
		}
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
	if (m_frameCt > getCurrentAnimation()->getDuration())
		m_frameCt = 0;
#endif
}

void MeshManager::decrementFrame()
{
	m_frameCt--;
#if USE_COLLADA
	if (m_frameCt < 0)
		m_frameCt = 24;
#else
	if (m_frameCt < 0)
		m_frameCt = getCurrentAnimation()->getDuration();
#endif
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

void MeshManager::setCurrentAnimation(int index)
{
	if (m_animations.size() > index)
		m_currentAnimation = m_animations[index];
	else
		m_currentAnimation = nullptr;
}

Animation* MeshManager::findAnimation(const QString& name)
{
	auto animIter = std::find_if(m_animations.begin(), m_animations.end(), [&](auto anim) {return anim->getName() == name; });
	if (animIter != m_animations.end())
		*animIter;

	return false;
}