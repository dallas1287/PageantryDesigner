#pragma once
#include <QVector2D>
#include <QVector3D>

const QVector3D X(1.0f, 0.0f, 0.0f);
const QVector3D Y(0.0f, 1.0f, 0.0f);
const QVector3D Z(0.0f, 0.0f, 1.0f);

const QVector4D X4(1.0f, 0.0f, 0.0f, 0.0f);
const QVector4D Y4(0.0f, 1.0f, 0.0f, 0.0f);
const QVector4D Z4(0.0f, 0.0f, 1.0f, 0.0f);
const QVector4D W4(0.0f, 0.0f, 0.0f, 1.0f);


struct VertexData
{
	VertexData(QVector3D position = QVector3D(), QVector2D texCoord = QVector2D(), QVector3D norm = QVector3D()) : position(position), texCoord(texCoord), normal(norm) {};
	~VertexData() {};
	QVector3D position;
	QVector2D texCoord;
	QVector3D normal;
};

const std::vector<VertexData> QuadTemplate =
{
	{ QVector3D(X + Y), QVector2D(1.0f, 1.0f) }, // top right
	{ QVector3D(X - Y), QVector2D(1.0f, 0.0f) }, // bottom right
	{ QVector3D(-X - Y), QVector2D(0.0f, 0.0f) }, // bottom left
	{ QVector3D(-X + Y), QVector2D(0.0f, 1.0f) }  // top left 
};

const std::vector<GLushort> QuadIndices =
{
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

const std::vector<GLushort> QuadOutlineIndices =
{
	0, 1,
	1, 2,
	2, 3,
	3, 0
};

namespace GridMaker
{
	namespace Lines
	{
		static void makeQuadOutline(std::vector<VertexData>& vertices, std::vector<GLushort>& indices)
		{
			vertices.insert(vertices.end(), QuadTemplate.begin(), QuadTemplate.end());
			indices.insert(indices.end(), QuadOutlineIndices.begin(), QuadOutlineIndices.end());
		}

		static void makeVerticalLines(int count, std::vector<VertexData>& vertices, std::vector<GLushort>& indices)
		{
			auto max = std::max_element(indices.begin(), indices.end());
			int ct = *max + 1;
			for (int i = 0; i < count; ++i)
			{
				vertices.push_back(VertexData((X * i / count) + Y));
				vertices.push_back(VertexData((X * i / count) + -(1.0f * Y)));
				vertices.push_back(VertexData(((-1.0f * X) * i / count) + Y));
				vertices.push_back(VertexData( ( (-1.0f * X) * i / count ) + -(1.0f * Y) ));
				for (int i = 0; i < 4; ++i)
					indices.push_back(ct++);
			}
		}

		static void makeHorizontalLines(int count, std::vector<VertexData> & vertices, std::vector<GLushort> & indices)
		{
			auto max = std::max_element(indices.begin(), indices.end());
			int ct = *max + 1;
			for (int i = 0; i < count; ++i)
			{
				vertices.push_back(VertexData(X + (Y * i / count)));
				vertices.push_back(VertexData(-(1.0f * X) + (Y * i / count)));
				vertices.push_back(VertexData(X + ((-1.0f * Y) * i / count)));
				vertices.push_back(VertexData((-1.0f * X) + ((-1.0f * Y) * i / count)));
				for (int i = 0; i < 4; ++i)
					indices.push_back(ct++);
			}
		}

		static void makeGridLines(int count, std::vector<VertexData>& vertices, std::vector<GLushort>& indices)
		{
			makeQuadOutline(vertices, indices);
			makeVerticalLines(count, vertices, indices);
			makeHorizontalLines(count, vertices, indices);
		}
	}

	namespace Quad
	{
		static void createQuad(std::vector<VertexData>& vertices, std::vector<GLushort>& indices)
		{
			vertices.insert(vertices.end(), QuadTemplate.begin(), QuadTemplate.end());
			indices.insert(indices.end(), QuadIndices.begin(), QuadIndices.end());
		}
	}
};
