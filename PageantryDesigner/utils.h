#pragma once
#include "Includes/assimp/scene.h"
#include <QMatrix4x4>

QMatrix4x4 convertTransformMatrix(aiMatrix4x4& in);

QString matrixToString(const QMatrix4x4& mat);
std::string matrixToStdString(const QMatrix4x4& mat);

QMatrix4x4 translationVectorToMatrix(QVector3D& vector);
QMatrix4x4 scalingVectorToMatrix(QVector3D& vector);

std::string vectorToStdString(const QVector3D& vector, int precision = 2);