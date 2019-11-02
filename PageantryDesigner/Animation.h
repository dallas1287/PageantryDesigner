#pragma once
#include "Includes/assimp/scene.h"
#include <QString>
#include <QVector3D>
#include <QQuaternion>
#include <QMatrix4x4>
#include <vector>
#include "Bone.h"

struct VectorKey
{
	float time;
	QVector3D value;
	QMatrix4x4 matrix;
};

struct QuaternionKey
{
	float time;
	QQuaternion value;
	QMatrix4x4 matrix;
};

struct TransformKey
{
	float time;
	QMatrix4x4 matrix;
};


class AnimationNode
{
public:
	AnimationNode() {};
	AnimationNode(aiNodeAnim* node);
	~AnimationNode();
	bool initialize();
	const QString& getName() const { return m_name; }
	std::vector<TransformKey>& getTransformKeys() { return m_transformKeys; }
	std::vector<VectorKey>& getPositionKeys() { return m_positionKeys; }
	std::vector<QuaternionKey>& getRotationKeys() { return m_rotationKeys; }
	std::vector<VectorKey>& getScalingKeys() { return m_scalingKeys; }
	bool getClosestTransform(int frame, QMatrix4x4& mat);

	QQuaternion m_basis;

private:
	void buildTransformKeys();

	aiNodeAnim* m_ref;
	QString m_name;
	Bone* m_refBone;
	std::vector<VectorKey> m_positionKeys;
	std::vector<QuaternionKey> m_rotationKeys;
	std::vector<VectorKey> m_scalingKeys;
	std::vector<TransformKey> m_transformKeys;
	
};

class Animation
{
public:
	Animation(aiAnimation* ref);
	~Animation();
	bool initialize();
	float getDuration() { return m_duration; }
	float getFPS() { return m_fps; }
	int getChannelCount() { return m_numChannels; }
	std::vector<AnimationNode*>& getAnimNodes() { return m_animNodes; }
	bool findAnimationNode(const QString& name, AnimationNode*& node);

private:
	aiAnimation* m_animRef;
	QString m_name;
	float m_duration;
	float m_fps;
	int m_numChannels;
	std::vector<AnimationNode*> m_animNodes;
};

