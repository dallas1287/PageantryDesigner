#include "BoneRig.h"
#include <QDebug>
#include "utils.h"
#include "Mesh.h"

bool BoneRig::buildSkeleton(aiNode* rootBone)
{
	if (!rootBone)
		return false;

	buildSkeletonRecursively(rootBone);

	m_rootBone = rootBone;
	return true;
}

void BoneRig::buildSkeletonRecursively(aiNode* bone)
{
	for (int i = 0; i < bone->mNumChildren; ++i)
	{
		buildSkeletonRecursively(bone->mChildren[i]);
		m_skeletonMap[QString(bone->mChildren[i]->mName.C_Str())] = bone->mChildren[i];
	}
}

void BoneRig::buildVertexTransforms()
{
	if (m_parent->getVertexData().size() != m_boneData.size())
	{
		qDebug() << "MISMATCHED data containers!!!";
		return;
	}

	for (int i = 0; i < m_boneData.size(); ++i)		
		m_boneData[i].FinalTransform = &m_parent->getVertexData()[i].transform;
}

void BoneRig::getAllChildren(const QString& bone, std::vector<aiNode*>& children)
{
	if (m_skeletonMap.find(bone) == m_skeletonMap.end())
		return;

	aiNode* boneNode = m_skeletonMap[bone];
	children.push_back(boneNode);
	getAllChildrenRecursively(boneNode, children);
}

void BoneRig::getAllChildrenRecursively(aiNode* boneNode, std::vector<aiNode*>& children)
{
	for (int i = 0; i < boneNode->mNumChildren; ++i)
	{
		QString name(boneNode->mChildren[i]->mName.data);
		getAllChildrenRecursively(boneNode->mChildren[i], children);
		children.push_back(boneNode->mChildren[i]);
	}
}

void BoneRig::getAllParents(const QString& bone, std::vector<aiNode*>& parents)
{
	if (m_skeletonMap.find(bone) == m_skeletonMap.end())
		return;

	aiNode* boneNode = m_skeletonMap[bone];
	aiNode* parent = boneNode->mParent;
	while (parent && parent != m_rootBone->mParent) 
	{
		parents.push_back(parent);
		parent = parent->mParent;
	}
}

void BoneRig::moveBone(const QString& bone, const QVector3D& location)
{
	std::vector<aiNode*> children;

	getAllChildren(bone, children);

	QMatrix4x4 gTransform;
	//gTransform.translate(location);
	gTransform.rotate(90.0, QVector3D(0.0, 1.0, 0.0));
	for (auto child : children)
	{
		Bone* childBone = m_parent->findDeformBone(QString(child->mName.data));
		if (childBone)
		{
			childBone->setModified();
			childBone->setTransform(gTransform);
		}
	}

	for (auto bd : m_boneData)
		bd.transformFromBones();
}

QMatrix4x4 BoneRig::getGlobalTransformation(std::vector<aiNode*>& parents)
{
	getTransformDebug(m_sceneRoot);
	QMatrix4x4 globalTransform;
	for (auto parent : parents)
	{
		getTransformDebug(parent);
		QMatrix4x4 converted = convertTransformMatrix(parent->mTransformation);
		globalTransform *= converted;
	}

	return globalTransform;
}

void BoneRig::getTransformDebug(aiNode* parent)
{
	qDebug() << "Name: " << parent->mName.data;
	QMatrix4x4 transform = convertTransformMatrix(parent->mTransformation);
	float* data = transform.data();
	qDebug() << data[0] << " " << data[1] << " " << data[2] << " " << data[3];
	qDebug() << data[4] << " " << data[5] << " " << data[6] << " " << data[7];
	qDebug() << data[8] << " " << data[9] << " " << data[10] << " " << data[11];
	qDebug() << data[12] << " " << data[13] << " " << data[14] << " " << data[15] << "\n";
}

void BoneRig::MeshTransformTest()
{
	QMatrix4x4 temp;
	temp.translate(1.0, 0.0, 0.0);
	qDebug() << "Template Translated: " << temp;
	std::vector<VertexData> firstVertexData, secondVertexData;

	firstVertexData.assign(m_parent->getVertexData().size(), VertexData());
	secondVertexData.assign(m_parent->getVertexData().size(), VertexData());

	for (int i = 0; i < m_parent->getVertexData().size(); ++i)
		firstVertexData[i].position = m_parent->getVertexData()[i].transform * m_parent->getVertexData()[i].position;

	moveBone("Root", QVector3D(1.0, 0.0, 0.0));
	
	for (int i = 0; i < m_parent->getVertexData().size(); ++i)
		secondVertexData[i].position = m_parent->getVertexData()[i].transform * m_parent->getVertexData()[i].position;

	for (int i = 0; i < secondVertexData.size(); ++i)
	{
		qDebug() << "Element: " << i;
		qDebug() << "Before: " << firstVertexData[i].position;
		qDebug() << "After: " << secondVertexData[i].position << "\n";
	}
}