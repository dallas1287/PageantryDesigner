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
		posKey.matrix = translationVectorToMatrix(posKey.value);
		QVector4D pt(0.0, 1.0, 0.0, 1.0);
		QVector4D testpt = posKey.matrix * pt;
		m_positionKeys.push_back(posKey);
	}

	//build rotation keys
	for (int j = 0; j < m_ref->mNumRotationKeys; ++j)
	{
		aiQuatKey key = m_ref->mRotationKeys[j];
		
		QuaternionKey rotKey;
		rotKey.time = key.mTime;
		rotKey.value = QQuaternion(QVector4D(key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w));
		rotKey.matrix = QMatrix4x4(rotKey.value.toRotationMatrix());
		m_rotationKeys.push_back(rotKey);
	}

	//scaling keys
	for (int i = 0; i < m_ref->mNumScalingKeys; ++i)
	{
		aiVectorKey key = m_ref->mScalingKeys[i];
		VectorKey scaleKey;
		scaleKey.time = key.mTime;
		//this is dumb and needs to be rectified with how things are imported
		if (key.mValue.x < 100.0)
			scaleKey.value = QVector3D(key.mValue.x, key.mValue.y, key.mValue.z);
		else
			scaleKey.value = QVector3D(1.0, 1.0, 1.0);

		scaleKey.matrix = scalingVectorToMatrix(scaleKey.value);
		m_scalingKeys.push_back(scaleKey);
	}

	if (m_positionKeys.size() != m_rotationKeys.size() || m_rotationKeys.size() != m_scalingKeys.size() || m_positionKeys.size() != m_scalingKeys.size())
	{
		//TODO: handle this error more completely
		qDebug() << "Key vectors aren't equal sized";
		return false;
	}

	buildTransformKeys();

	return true;
}

void AnimationNode::buildTransformKeys()
{
	
	for (int i = 0; i < m_positionKeys.size(); ++i)
	{
		TransformKey key;
		key.time = m_positionKeys[i].time;

		QMatrix4x4 newmat;
		QQuaternion newq = m_rotationKeys[i].value* m_basis;

		key.matrix = m_positionKeys[i].matrix * m_rotationKeys[i].matrix /*QMatrix4x4(newq.toRotationMatrix())*/ * m_scalingKeys[i].matrix;
		m_transformKeys.push_back(key);
	}
}

bool AnimationNode::getClosestTransform(int frame, QMatrix4x4& mat)
{
	if (m_transformKeys.empty())
		return false;

	for (int i = 0; i < m_transformKeys.size(); ++i)
	{
		if (qRound(m_transformKeys[i].time) < frame)
			continue;
		if (qRound(m_transformKeys[i].time) == frame)
		{
			mat = m_transformKeys[i].matrix;
			return true;
		}
		if (qRound(m_transformKeys[i].time) > frame)
		{
			mat = m_transformKeys[i - 1].matrix;
			return true;
		}
	}
	mat = m_transformKeys.back().matrix;
	return true;
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

bool Animation::findAnimationNode(const QString& name, AnimationNode& node)
{
	auto n = std::find_if(m_animNodes.begin(), m_animNodes.end(), [&](auto node) {return node.getName() == name; });
	if (n != m_animNodes.end())
	{
		node = *n;
		return true;
	}

	return false;
}