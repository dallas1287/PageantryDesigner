#pragma once
#include <QString>
#include <unordered_map>
#include "Bone.h"

const QVector3D X(1.0f, 0.0f, 0.0f);
const QVector3D Y(0.0f, 1.0f, 0.0f);
const QVector3D Z(0.0f, 0.0f, 1.0f);

const QVector4D X4(1.0f, 0.0f, 0.0f, 0.0f);
const QVector4D Y4(0.0f, 1.0f, 0.0f, 0.0f);
const QVector4D Z4(0.0f, 0.0f, 1.0f, 0.0f);
const QVector4D W4(0.0f, 0.0f, 0.0f, 1.0f);

struct VertexData
{
	VertexData(QVector3D position = QVector3D(), QVector3D texCoord = QVector3D(), QVector3D norm = QVector3D(), 
		QVector4D col = QVector4D(), QMatrix4x4 trans = QMatrix4x4()) : 
		position(position), texCoord(texCoord), normal(norm), color(col), transform(trans) {};
	~VertexData() {};

	QVector3D position;
	QVector3D texCoord;
	QVector3D normal;
	QVector4D color;
	QMatrix4x4 transform;
};

typedef std::vector<VertexData> VertexDataPool;