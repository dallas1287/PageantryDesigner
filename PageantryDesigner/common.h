#pragma once
#include <QString>
const QVector3D X(1.0f, 0.0f, 0.0f);
const QVector3D Y(0.0f, 1.0f, 0.0f);
const QVector3D Z(0.0f, 0.0f, 1.0f);

const QVector4D X4(1.0f, 0.0f, 0.0f, 0.0f);
const QVector4D Y4(0.0f, 1.0f, 0.0f, 0.0f);
const QVector4D Z4(0.0f, 0.0f, 1.0f, 0.0f);
const QVector4D W4(0.0f, 0.0f, 0.0f, 1.0f);

struct VertexData
{
	VertexData(QVector3D position = QVector3D(), QVector2D texCoord = QVector2D(), QVector3D norm = QVector3D(), QMatrix4x4 trans = QMatrix4x4()) : 
		position(position), texCoord(texCoord), normal(norm), transform(trans) {};
	~VertexData() {};

	const QString printData()
	{
		const float* tdata = transform.constData();
		QString output = "Position: " + vectorToString(position) + QString("\n") +
			"Tex Coords: " + vectorToString(texCoord) + QString("\n") +
			"Normals: " + vectorToString(normal) + QString("\n") +
			"Transform: " + tdata[0] + " " + tdata[1] + " " + tdata[2] + " " + tdata[3] + QString("\n") +
			tdata[4] + " " + tdata[5] + " " + tdata[6] + " " + tdata[7] + QString("\n") +
			tdata[8] + " " + tdata[9] + " " + tdata[10] + " " + tdata[11] + QString("\n") +
			tdata[12] + " " + tdata[13] + " " + tdata[14] + " " + tdata[15];

		return output;
	}

	const QString vectorToString(const QVector2D& vect)
	{
		return QString::number(vect.x()) + " " + QString::number(vect.y());
	}

	const QString vectorToString(const QVector3D& vect)
	{
		return QString::number(vect.x()) + " " + QString::number(vect.y()) + " " + QString::number(vect.z());
	}

	QVector3D position;
	QVector2D texCoord;
	QVector3D normal;
	QMatrix4x4 transform;
};
