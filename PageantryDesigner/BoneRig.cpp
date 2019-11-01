#include "BoneRig.h"
#include <QDebug>
#include "utils.h"
#include "Mesh.h"
#include <fstream>

bool BoneRig::buildSkeleton(aiNode* rootNode)
{
	if (!rootNode)
		return false;

	buildSkeletonRecursively(rootNode);
	return true;
}

void BoneRig::buildSkeletonRecursively(aiNode* bone)
{
	for (int i = 0; i < bone->mNumChildren; ++i)
	{
		buildSkeletonRecursively(bone->mChildren[i]);
		//if it has meshes it's not a bone
		if (bone->mChildren[i]->mNumMeshes == 0)
			m_skeletonMap[QString(bone->mChildren[i]->mName.C_Str())] = bone->mChildren[i];
	}
}

aiNode* BoneRig::findBoneInSkeleton(const QString& bone)
{
	if (m_skeletonMap.find(bone) == m_skeletonMap.end())
		return nullptr;

	return m_skeletonMap[bone];
}

void BoneRig::getAllChildren(const QString& bone, std::vector<aiNode*>& children)
{
	aiNode* boneNode = findBoneInSkeleton(bone);
	if (boneNode)
	{
		children.push_back(boneNode);
		getAllChildrenRecursively(boneNode, children);
	}
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

/*void BoneRig::moveBone(const QString& bone, const QVector3D& location)
{
	std::vector<aiNode*> children;

	getAllChildren(bone, children);

	QMatrix4x4 gTransform;
	//gTransform.translate(location);
	gTransform.rotate(270.0, QVector3D(0.0, 1.0, 0.0));
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
}*/

/*static bool alreadyWritten = false;
void BoneRig::moveDirectly(const QString& bone)
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
	if (!alreadyWritten)
	{
		std::ofstream outfile("../logData.txt");
		std::ofstream transformfile("../logTransform.txt");
		for (auto bd : m_boneData)
		{
			QVector3D pos = m_parent->getVertexData()[bd.ID].position;
			QString originalPoint = QString::number(pos.x(), 'f', 2) + ", " + QString::number(pos.y(), 'f', 2) + ", " + QString::number(pos.z(), 'f', 2);
			std::string data = originalPoint.toLocal8Bit().constData();
			outfile << "Index: " << bd.ID << " " << data << "\t";

			if (bd.transformFromBones())
			{
				m_parent->getVertexData()[bd.ID].position = *bd.FinalTransform * pos;
				pos = m_parent->getVertexData()[bd.ID].position;
				QString originalPoint = QString::number(pos.x(), 'f', 2) + ", " + QString::number(pos.y(), 'f', 2) + ", " + QString::number(pos.z(), 'f', 2);
				data = originalPoint.toLocal8Bit().constData();
				outfile << data;
				transformfile << "Index: " << bd.ID << std::endl << matrixToString(*bd.FinalTransform).toLocal8Bit().constData() << std::endl;
			}
			outfile << std::endl;		
		}
		outfile.close();
		transformfile.close();
		alreadyWritten = true;
	}
}*/