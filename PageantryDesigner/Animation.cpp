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
	for (int i = 0; i < m_positionKeys.size(); ++i)
	{
		m_positionKeys[i].matrix = translationVectorToMatrix(m_positionKeys[i].value);
		m_rotationKeys[i].matrix = QMatrix4x4(m_rotationKeys[i].value.toRotationMatrix());
		m_scalingKeys[i].matrix = scalingVectorToMatrix(m_scalingKeys[i].value);
		m_transforms.push_back(m_positionKeys[i].matrix * m_rotationKeys[i].matrix * m_scalingKeys[i].matrix);
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

AnimationNode& Animation::findAnimationNode(const QString& name)
{
	auto node = std::find_if(m_animNodes.begin(), m_animNodes.end(), [&](auto node) {return node.getName() == name; });
	if (node != m_animNodes.end())
		return *node;

	//TODO: fix this, this is dumb
	return *m_animNodes.begin();
}

void Animation::buildTransforms()
{
	for (auto anim : m_animNodes)
	{
		if (anim.getName() == "Armature")
			continue;

		for (int i = 0; i < anim.getPositionKeys().size(); ++i)
			anim.getTransforms()[i];
	}
}