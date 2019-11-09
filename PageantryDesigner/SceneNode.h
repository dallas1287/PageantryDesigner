#pragma once
#include "Includes/assimp/scene.h"
#include <vector>
#include <QString>
#include <QMatrix4x4>

class SceneNode
{
public:
	SceneNode() {}
	SceneNode(aiNode* refNode);
	~SceneNode();
	const QString& getName() const { return m_name; }
	SceneNode* getParent() { return m_parent; }
	bool HasParent() const { return m_parent != nullptr; }
	std::vector<SceneNode*>& Children() { return m_children; }
	void setParent(SceneNode* parent) { m_parent = parent; }
	QMatrix4x4& getTransformMatrix() { return m_transformMatrix; }

	//this is dumb and necessary because blender exports units strangely. I can fix this before import
	//if i change the blender scale units but this will probably be a nice convenience function later on in the gui
	void adjustScaling(float factor) { m_transformMatrix.scale(factor); }

private:
	void init();

	aiNode* m_refNode = nullptr;
	QString m_name;
	SceneNode* m_parent;
	std::vector<SceneNode*> m_children;
	QMatrix4x4 m_transformMatrix;
	std::vector<unsigned int> m_meshes;

};

