#include "MeshManager.h"
#include "GraphicsPanel.h"

MeshManager::~MeshManager()
{
	for (auto meshObj : m_meshPool)
		delete meshObj;
	for (auto node : m_nodeMap)
		delete node.second;
	for (auto anim : m_animations)
		delete anim;
	for (auto light : m_sceneLights)
		delete light;
	for (auto cam : m_sceneCameras)
		delete cam;
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
	for (auto light : m_sceneLights)
		delete light;
	m_sceneLights.clear();
	for (auto cam : m_sceneCameras)
		delete cam;
	m_sceneCameras.clear();
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
	createSceneNodes(scene->mRootNode);

	if (scene->HasCameras())
	{
		for (int i = 0; i < scene->mNumCameras; ++i)
			m_sceneCameras.emplace_back(new SceneCamera(scene->mCameras[i]));
	}

	if (scene->HasLights())
	{
		for (int i = 0; i < scene->mNumLights; ++i)
			m_sceneLights.emplace_back(new SceneLight(scene->mLights[i]));
	}

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

	auto nameIter = names.begin();
	while(nameIter != names.end())
	{
		bool foundBone = false;
		for (auto mesh : getMeshes())
		{
			if (!mesh->isRigged())
				continue;
			foundBone = sceneNodeContainsBone(*nameIter, mesh);
		}
		if (!foundBone)
			nameIter = names.erase(nameIter);
		else
			++nameIter;
	}
	if(!names.empty())
		return findSceneNode(names.front());
	return nullptr;
}

bool MeshManager::sceneNodeContainsBone(const QString& name, MeshObject* mesh)
{
	SceneNode* node = findSceneNode(name);
	if (!node || !mesh)
		return false;

	if (!mesh->isRigged())
		return false;

	if (hasDeformBoneInHierarchy(node, mesh))
		return true;
	return false;
}

bool MeshManager::hasDeformBoneInHierarchy(SceneNode* node, MeshObject* mesh)
{
	if (!node || !mesh)
		return false;

	for (auto child : node->Children())
	{
		//this node is a deform bone
		if (mesh->findDeformBone(child->getName()))
			return true;
		//runs through all the children of this node 
		if (hasDeformBoneInHierarchy(child, mesh))
			return true;
	}

	return false;
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

	SceneNode* sn = findArmatureNode(scene);
	if (!sn)
		return;

	if (!m_boneRig)
		m_boneRig.reset(new BoneRig(this));

	aiNode* rig = scene->mRootNode->FindNode(sn->getName().toLocal8Bit().constData());
	if (rig)
	{
		m_boneRig->setSceneRoot(scene->mRootNode);
		m_boneRig->setRigRoot(sn);
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
		meshObj->initBuffers(meshObj->getVertexData(), meshObj->getIndices());
	}

	if (!((MeshRenderer*)m_parent)->getParent()->isPaused())
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