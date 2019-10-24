#pragma once
#include "Includes/assimp/scene.h"
#include <QString>
#include <QMatrix4x4>
#include <unordered_map>

typedef std::unordered_map<int, float> VertexWeightMap;

class Bone
{
public:
	Bone() {};
	~Bone() {};
	void initBone(aiBone* refBone);
	const QString& getName() const { return m_name; }
	const unsigned int WeightCount() const { return m_numWeights; }
	const QMatrix4x4& OffsetMatrix() const { return m_OffsetMatrix; }
	VertexWeightMap& getVertexWeightMap() { return m_vertexWeightMap; }
	float findVertexWeight(unsigned int id);
	bool isModified() { return m_modified; }
	void setModified(bool state = true) { m_modified = state; }
	void setTransform(QMatrix4x4& transform) { m_transformMatrix = transform; }
	QMatrix4x4& getTransform() { return m_transformMatrix; }

private:
	QString m_name;
	unsigned int m_numWeights;
	QMatrix4x4 m_OffsetMatrix;
	VertexWeightMap m_vertexWeightMap;
	bool m_modified = false;
	QMatrix4x4 m_transformMatrix;
};
