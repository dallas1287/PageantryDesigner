#pragma once
#include <QString>
#include <unordered_map>
#include "Bone.h"

const QVector3D X(1.0, 0.0, 0.0);
const QVector3D Y(0.0, 1.0, 0.0);
const QVector3D Z(0.0, 0.0, 1.0);

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
	VertexData& operator=(const VertexData& other)
	{
		position = other.position;
		texCoord = other.texCoord;
		normal = other.normal;
		color = other.color;
		transform = other.transform;
		return *this;
	}

	QVector3D position;
	QVector3D texCoord;
	QVector3D normal;
	QVector4D color;
	QMatrix4x4 transform;
};
typedef std::vector<VertexData> VertexDataPool;

struct MaterialData
{
	MaterialData(QVector3D amb = QVector3D(), QVector3D dif = QVector3D(), QVector3D spec = QVector3D(), float shine = 1.0) : ambient(amb), diffuse(dif), specular(spec), shininess(shine) {}
	~MaterialData() {}
	MaterialData& operator=(const MaterialData& other)
	{
		ambient = other.ambient;
		diffuse = other.diffuse;
		specular = other.specular;
		shininess = other.shininess;
		return *this;
	}

	QVector3D ambient;
	QVector3D diffuse;
	QVector3D specular;
	float shininess = 1.0;
};
typedef std::vector<MaterialData> MaterialDataPool;

struct LightData
{
	enum TYPE
	{
		Directional,
		Point,
		Spotlight,
		Area
	};

	LightData(QVector3D amb = QVector3D(), QVector3D dif = QVector3D(), QVector3D spec = QVector3D(), QVector3D pos = QVector3D(), QVector3D dir = QVector3D(), 
		float con = 0.0, float lin = 0.0, float quad = 0.0) : 
		ambient(amb), diffuse(dif), specular(spec), position(pos), direction(dir), attenuationConstant(con), attenuationLinear(lin), attenuationQuadratic(quad) {}
	~LightData() {}
	LightData& operator=(const LightData& other)
	{
		ambient = other.ambient;
		diffuse = other.diffuse;
		specular = other.specular;
		position = other.position;
		direction = other.direction;
		attenuationConstant = other.attenuationConstant;
		attenuationLinear = other.attenuationLinear;
		attenuationQuadratic = other.attenuationQuadratic;
		type = other.type;
		return *this;
	}

	QVector3D ambient;
	QVector3D diffuse;
	QVector3D specular;
	QVector3D position;
	QVector3D direction;
	float attenuationConstant = 0.0;
	float attenuationLinear = 0.0;
	float attenuationQuadratic = 0.0;
	int type = TYPE::Directional;
};