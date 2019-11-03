#pragma once
#include "Includes/assimp/Importer.hpp"
#include "Includes/assimp/scene.h"
#include "Includes/assimp/postprocess.h"
#include "GraphicsObject.h"
#include "ShapeCreator.h"
#include "Bone.h"
#include "BoneRig.h"
#include "Animation.h"
#include <unordered_map>
#include "ItemRenderer.h"
#include "SceneNode.h"
#include <QOpenGLExtraFunctions>

class MeshObject : public GraphicsObject, protected QOpenGLExtraFunctions
{
public:
	MeshObject(aiMesh* ref);
	~MeshObject();
	void initialize();
	void initializeBuffers();
	aiMesh* getMeshRef() { return m_meshRef; }
	std::vector<VertexData>& getVertexData() { return m_meshData; }
	std::vector<GLushort>& getIndices() { return m_indices; }
	std::vector<Bone*>& DeformBones() { return m_deformBones; }
	
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
	int numIndices = 0;
	aiMesh* m_meshRef = nullptr;
};

typedef std::vector<MeshObject*> MeshObjectPool;
typedef std::vector<Animation*> AnimationPool;
typedef std::map<QString, SceneNode*> NodeMap;

class MeshManager
{
public:
	MeshManager(ItemRenderer* parent): m_parent(parent), m_boneRig(BoneRig(this)) {};
	~MeshManager();

	bool import(const QString& path);
	BoneRig& getBoneRig() { return m_boneRig; }
	MeshObjectPool& getMeshes() { return m_meshPool; }
	MeshObject* getCurrentMesh() { return m_currentMesh; }
	void setCurrentMesh(MeshObject* meshObj) { m_currentMesh = meshObj; }
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

private:
	void createMeshes(const aiScene* scene);
	void createSkeleton(aiNode* root);
	void createSceneNodes(aiNode* root);
	void createSceneNodesRecursively(aiNode* node);
	void createSceneTreeRecursively(aiNode* node);
	void animateRecursively(aiNode* node, const QMatrix4x4& parentTransform);

	ItemRenderer* m_parent;
	Assimp::Importer m_importer;
	QMatrix4x4 m_globalTransform;
	BoneRig m_boneRig;
	NodeMap m_nodeMap;
	MeshObjectPool m_meshPool;
	MeshObject* m_currentMesh = nullptr;
	AnimationPool m_animations;
	Animation* m_currentAnimation = nullptr;
	int m_frameCt = 0;
};

