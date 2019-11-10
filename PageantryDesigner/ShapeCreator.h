#pragma once
#include <QVector2D>
#include <QVector3D>
#include "common.h"

const std::vector<VertexData> QuadTemplate =
{
	{ QVector3D(X + Y), QVector3D(1.0, 1.0, 0.0) }, // top right
	{ QVector3D(X - Y), QVector3D(1.0, 0.0, 0.0) }, // bottom right
	{ QVector3D(-X - Y), QVector3D(0.0, 0.0, 0.0) }, // bottom left
	{ QVector3D(-X + Y), QVector3D(0.0, 1.0, 0.0) }  // top left 
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

const std::vector<VertexData> CubeTemplate =
{
	//face 1 Z+
	{ QVector3D(X + Y + Z), QVector3D(1.0, 1.0, 0.0) }, // top right
	{ QVector3D(-X + Y + Z), QVector3D(1.0, 0.0, 0.0) }, // bottom right
	{ QVector3D(-X - Y + Z), QVector3D(0.0, 0.0, 0.0) }, // bottom left
	{ QVector3D(X - Y + Z), QVector3D(0.0, 1.0, 0.0) },  // top left 
	//face 2 Y-
	{ QVector3D(X - Y - Z), QVector3D(1.0, 1.0, 0.0) }, // top right
	{ QVector3D(X - Y + Z), QVector3D(1.0, 0.0, 0.0) }, // bottom right
	{ QVector3D(-X - Y + Z), QVector3D(0.0, 0.0, 0.0) }, // bottom left
	{ QVector3D(-X - Y - Z), QVector3D(0.0, 1.0, 0.0) },  // top left 
	//face 3 X-
	{ QVector3D(-X - Y - Z), QVector3D(1.0, 1.0, 0.0) }, // top right
	{ QVector3D(-X - Y + Z), QVector3D(1.0, 0.0, 0.0) }, // bottom right
	{ QVector3D(-X + Y + Z), QVector3D(0.0, 0.0, 0.0) }, // bottom left
	{ QVector3D(-X + Y - Z), QVector3D(0.0, 1.0, 0.0) },  // top left 
	//face 4 Z-
	{ QVector3D(-X + Y - Z), QVector3D(1.0, 1.0, 0.0) }, // top right
	{ QVector3D(X + Y - Z), QVector3D(1.0, 0.0, 0.0) }, // bottom right
	{ QVector3D(X - Y - Z), QVector3D(0.0, 0.0, 0.0) }, // bottom left
	{ QVector3D(-X - Y - Z), QVector3D(0.0, 1.0, 0.0) },  // top left 
	//face 5 X+
	{ QVector3D(X + Y - Z), QVector3D(1.0, 1.0, 0.0) }, // top right
	{ QVector3D(X + Y + Z), QVector3D(1.0, 0.0, 0.0) }, // bottom right
	{ QVector3D(X - Y + Z), QVector3D(0.0, 0.0, 0.0) }, // bottom left
	{ QVector3D(X - Y - Z), QVector3D(0.0, 1.0, 0.0) },  // top left 
	//face 6 Y+
	{ QVector3D(-X + Y - Z), QVector3D(1.0, 1.0, 0.0) }, // top right
	{ QVector3D(-X + Y + Z), QVector3D(1.0, 0.0, 0.0) }, // bottom right
	{ QVector3D(X + Y + Z), QVector3D(0.0, 0.0, 0.0) }, // bottom left
	{ QVector3D(X + Y - Z), QVector3D(0.0, 1.0, 0.0) },  // top left 


};

const std::vector<GLushort> CubeIndices =
{
	0,1,2,
	0,2,3,
	4,5,6,
	4,6,7,
	8,9,10,
	8,10,11,
	12,13,14,
	12,14,15,
	16,17,18,
	16,18,19,
	20,21,22,
	20,22,23
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

	namespace Cube
	{
		static void createCube(std::vector<VertexData>& vertices, std::vector<GLushort>& indices)
		{
			vertices.insert(vertices.end(), CubeTemplate.begin(), CubeTemplate.end());
			indices.insert(indices.end(), CubeIndices.begin(), CubeIndices.end());
		}
	}
};
