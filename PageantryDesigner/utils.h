#pragma once
#include "Includes/assimp/scene.h"
#include <QMatrix4x4>
#include "SceneCamera.h"
#include "Mesh.h"

QMatrix4x4 convertTransformMatrix(aiMatrix4x4& in);

QString matrixToString(const QMatrix4x4& mat);
std::string matrixToStdString(const QMatrix4x4& mat);

QMatrix4x4 translationVectorToMatrix(QVector3D& vector);
QMatrix4x4 scalingVectorToMatrix(QVector3D& vector);

QString vectorToString(const aiVector3D& vector, int precision = 2);
std::string vectorToStdString(const aiVector3D& vector, int precision = 2);
QString vectorToString(const QVector3D& vector, int precision = 2);
std::string vectorToStdString(const QVector3D& vector, int precision = 2);

QString quaternionToString(const aiQuaternion& quad, int precision = 2);
std::string quaternionToStdString(const aiQuaternion& quad, int precision = 2);

QString quaternionToString(const QQuaternion& quat, int precision = 2);
std::string quaternionToStdString(const QQuaternion& quat, int precision = 2);

std::string LogCamera(SceneCamera& cam);
std::string LogVertex(MeshObject* meshObj, int index);
std::string LogTransform(MeshManager& mm, int index);