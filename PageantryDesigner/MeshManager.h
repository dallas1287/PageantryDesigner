#pragma once
#include "Includes/assimp/Importer.hpp"
#include "Includes/assimp/scene.h"
#include "Includes/assimp/postprocess.h"
#include "ItemRenderer.h"
#include "MeshObject.h"
#include "Animation.h"
#include "SceneNode.h"
#include "BoneRig.h"
#include "SceneLight.h"
#include <QString>
#include <vector>

typedef std::vector<MeshObject*> MeshObjectPool;
typedef std::vector<Animation*> AnimationPool;
typedef std::vector<SceneLight*> LightPool;
typedef std::vector<SceneCamera*> CameraPool;
typedef std::map<QString, SceneNode*> NodeMap;

class MeshManager
{
public:
	MeshManager(ItemRenderer* parent): m_parent(parent) {};
	~MeshManager();

	bool import(const QString& path);
	void resetData();
	BoneRig* getBoneRig() { return m_boneRig.get(); }
	MeshObjectPool& getMeshes() { return m_meshPool; }
	MeshObject* getCurrentMesh() { return m_currentMesh; }
	void setCurrentMesh(MeshObject* meshObj) { m_currentMesh = meshObj; }
	MeshObject* findMesh(const QString& name);
	QMatrix4x4& GlobalTransform() { return m_globalTransform; }
	QMatrix4x4 GlobalInverseTransform() { return m_globalTransform.inverted(); }
	AnimationPool& getAnimations() { return m_animations; }
	Animation* findAnimation(const QString& name);
	Animation* getCurrentAnimation() { return m_currentAnimation; }
	void setCurrentAnimation(int index);
	void setCurrentAnimation(const QString& name) { m_currentAnimation = findAnimation(name); }
	void animate();
	void incrementFrame();
	void decrementFrame();
	int getFrameCt() { return m_frameCt; }
	void setFrameCt(int frame) { m_frameCt = frame; }
	SceneNode* findSceneNode(const QString& name);
	SceneNode* findArmatureNode(const aiScene* scene);

private:
	void createMeshes(const aiScene* scene);
	void createSkeleton(const aiScene* scene);
	void createSceneNodes(aiNode* root);
	void createSceneNodesRecursively(aiNode* node);
	void createSceneTreeRecursively(aiNode* node);
	void animateRecursively(aiNode* node, const QMatrix4x4& parentTransform);
	bool sceneNodeContainsBone(const QString& name, MeshObject* mesh);
	bool MeshManager::hasDeformBoneInHierarchy(SceneNode* node, MeshObject* mesh);

	ItemRenderer* m_parent;
	Assimp::Importer m_importer;
	QMatrix4x4 m_globalTransform;
	std::unique_ptr<BoneRig> m_boneRig;
	NodeMap m_nodeMap;
	MeshObjectPool m_meshPool;
	MeshObject* m_currentMesh = nullptr;
	AnimationPool m_animations;
	Animation* m_currentAnimation = nullptr;
	LightPool m_sceneLights;
	CameraPool m_sceneCameras;
	int m_frameCt = 0;
};

