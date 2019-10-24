#include "Animation.h"
#include <QDebug>
#include <QMatrix4x4>
#include <QTransform>
#include "utils.h"

AnimationNode::AnimationNode(aiNodeAnim* ref) : m_ref(ref)
{
	initialize();
}

AnimationNode::~AnimationNode()
{
}

bool AnimationNode::initialize()
{
	if(!m_ref)
	{
		//TODO: handle this more completely
		qDebug() << "NO ANIMATION NODE REFERENCE";
		return false;
	}

	m_name = m_ref->mNodeName.data;

	//build position keys
	for (int i = 0; i < m_ref->mNumPositionKeys; ++i)
	{
		aiVectorKey key = m_ref->mPositionKeys[i];
		VectorKey posKey;
		posKey.time = key.mTime;
		posKey.value = QVector3D (key.mValue.x, key.mValue.y, key.mValue.z);
		m_positionKeys.push_back(posKey);
	}

	//build rotation keys
	for (int j = 0; j < m_ref->mNumRotationKeys; ++j)
	{
		aiQuatKey key = m_ref->mRotationKeys[j];
		QuaternionKey rotKey;
		rotKey.time = key.mTime;
		rotKey.value = QQuaternion(QVector4D(key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w));
		m_rotationKeys.push_back(rotKey);
	}

	//scaling keys
	for (int i = 0; i < m_ref->mNumScalingKeys; ++i)
	{
		aiVectorKey key = m_ref->mScalingKeys[i];
		VectorKey scaleKey;
		scaleKey.time = key.mTime;
		scaleKey.value = QVector3D(key.mValue.x, key.mValue.y, key.mValue.z);
		m_scalingKeys.push_back(scaleKey);
	}

	if (m_positionKeys.size() != m_rotationKeys.size() || m_rotationKeys.size() != m_scalingKeys.size() || m_positionKeys.size() != m_scalingKeys.size())
	{
		//TODO: handle this error more completely
		qDebug() << "Key vectors aren't equal sized";
		return false;
	}

	buildTransforms();

	return true;
}

void AnimationNode::buildTransforms()
{
	QMatrix4x4 translateMat, rotateMat, scaleMat;

	for (int i = 0; i < m_positionKeys.size(); ++i)
	{
		translateMat = translationVectorToMatrix(m_positionKeys[i].value);
		rotateMat = QMatrix4x4(m_rotationKeys[i].value.toRotationMatrix());
		scaleMat = scalingVectorToMatrix(m_scalingKeys[i].value);
		m_transforms.push_back(translateMat * rotateMat * scaleMat);
	}
}

Animation::Animation(aiAnimation* ref) : m_animRef(ref)
{
	initialize();
}

Animation::~Animation()
{
}

bool Animation::initialize()
{
	if (!m_animRef)
	{
		//TODO: handle this more completely
		qDebug() << "NO ANIMATION REFERENCE";
		return false;
	}

	m_duration = m_animRef->mDuration;
	m_fps = m_animRef->mTicksPerSecond;
	m_numChannels = m_animRef->mNumChannels;

	for (int i = 0; i < m_animRef->mNumChannels; ++i)
	{
		AnimationNode node(m_animRef->mChannels[i]);
		m_animNodes.push_back(node);
	}

	return true;
}