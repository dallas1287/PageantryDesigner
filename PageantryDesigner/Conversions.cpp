#include "utils.h"

QMatrix4x4 convertTransformMatrix(aiMatrix4x4& in)
{
	float vals[16] =
	{
		in.a1, in.a2, in.a3, in.a4,
		in.b1, in.b2, in.b3, in.b4,
		in.c1, in.c2, in.c3, in.c4,
		in.d1, in.d2, in.d3, in.d4
	};

	return QMatrix4x4(vals);
}

QString matrixToString(const QMatrix4x4& mat)
{
	const float* data = mat.data();
	QString matString;
	for (int i = 0; i < 16; ++i)
	{
		matString += QString::number(data[i], 'f', 2) + " ";
		if ((i + 1) % 4 == 0)
			matString += "\n";
	}
	return matString;
}

std::string matrixToStdString(const QMatrix4x4& mat)
{
	QString str = matrixToString(mat);
	return str.toLocal8Bit().constData();
}

QMatrix4x4 translationVectorToMatrix(QVector3D& vector)
{
	QMatrix4x4 mat;
	QVector4D translation(vector.x(), vector.y(), vector.z(), 1.0);
	mat.setColumn(3, translation);
	return mat;
}

QMatrix4x4 scalingVectorToMatrix(QVector3D& vector)
{
	QMatrix4x4 mat;
	//see comment above setting row counterintuitively because it's actually the column,
	//most likely due to them being considered "row major"
	mat.setRow(0, mat.column(0) * vector.x());
	mat.setRow(1, mat.column(1) * vector.y());
	mat.setRow(2, mat.column(2) * vector.z());
	return mat;
}

QString vectorToString(const aiVector3D& vector, int precision)
{
	return QString::number(vector.x, 'f', precision) + ", " + QString::number(vector.y, 'f', precision) + ", " + QString::number(vector.z, 'f', precision);
}

std::string vectorToStdString(const aiVector3D& vector, int precision)
{
	QString transformStr = vectorToString(vector, precision);
	return transformStr.toLocal8Bit().constData();
}

QString vectorToString(const QVector3D& vector, int precision)
{
	return QString::number(vector.x(), 'f', precision) + ", " + QString::number(vector.y(), 'f', precision) + ", " + QString::number(vector.z(), 'f', precision);
}

std::string vectorToStdString(const QVector3D& vector, int precision)
{
	QString transformStr = vectorToString(vector, precision);
	return transformStr.toLocal8Bit().constData();
}

QString quaternionToString(const aiQuaternion& quat, int precision)
{
	return QString::number(quat.w, 'f', precision) + ", " + QString::number(quat.x, 'f', precision) + ", " + QString::number(quat.y, 'f', precision) + QString::number(quat.z, 'f', precision);
}

std::string quaternionToStdString(const aiQuaternion& quat, int precision)
{
	QString quatStr = quaternionToString(quat, precision);
	return quatStr.toLocal8Bit().constData();
}

QString quaternionToString(const QQuaternion& quat, int precision)
{
	return QString::number(quat.scalar(), 'f', precision) + ", " + QString::number(quat.x(), 'f', precision) + ", " + QString::number(quat.y(), 'f', precision) + ", " + QString::number(quat.z(), 'f', precision);
}

std::string quaternionToStdString(const QQuaternion& quat, int precision)
{
	QString quatStr = quaternionToString(quat, precision);
	return quatStr.toLocal8Bit().constData();
}

std::string LogCamera(SceneCamera& cam)
{
	return "Perspective:\n" + matrixToStdString(cam.Perspective()) +
		"View:\n"  + matrixToStdString(cam.View());
}

std::string LogVertex(MeshObject* meshObj, int index)
{
	return "Position: " + vectorToStdString(meshObj->getVertexData()[index].position) + "\n";
}

std::string LogTransform(MeshManager& mm, int index)
{
	return "Transform:\n" + matrixToStdString(*(mm.getMeshes()[0]->getBoneData()[index].FinalTransform));
}