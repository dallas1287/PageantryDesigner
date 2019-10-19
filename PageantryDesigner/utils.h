#pragma once
#include "Includes/assimp/scene.h"
#include <QMatrix4x4>

void convertTransformMatrix(aiMatrix4x4& in, QMatrix4x4& out);
QMatrix4x4 convertTransformMatrix(aiMatrix4x4& in);