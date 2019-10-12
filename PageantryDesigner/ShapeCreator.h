#pragma once
#include <QVector2D>
#include <QVector3D>

const QVector3D X(1.0f, 0.0f, 0.0f);
const QVector3D Y(0.0f, 1.0f, 0.0f);
const QVector3D Z(0.0f, 0.0f, 1.0f);

struct VertexData
{
	VertexData(QVector3D position = QVector3D(), QVector2D texCoord = QVector2D(), QVector3D color = QVector3D()) : position(position), texCoord(texCoord), color(color) {};
	~VertexData() {};
	QVector3D position;
	QVector2D texCoord;
	QVector3D color;
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

namespace GridMaker
{
	namespace Lines
	{
		static void makeQuadOutline(std::vector<QVector3D>& points, std::vector<GLushort>& indices)
		{
			points.push_back(-1.0f * X + Y); //top left
			points.push_back(X + Y); // top Right
			points.push_back(X + -1.0f * Y); // bottom right
			points.push_back(-1.0f * (X + Y)); // bottom left

			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(1);
			indices.push_back(2);
			indices.push_back(2);
			indices.push_back(3);
			indices.push_back(3);
			indices.push_back(0);
		}

		static void makeVerticalLines(int count, std::vector<QVector3D>& points, std::vector<GLushort>& indices)
		{
			auto max = std::max_element(indices.begin(), indices.end());
			int ct = *max + 1;
			for (int i = 0; i < count; ++i)
			{
				points.push_back((X * i / count) + Y); //upper point +X 
				points.push_back((X * i / count) + -(1.0f * Y)); //lower point +X
				points.push_back(((-1.0f * X) * i / count) + Y); //upper point -X
				points.push_back(((-1.0f * X) * i / count) + -(1.0f * Y)); //lower point -X
				for (int i = 0; i < 4; ++i)
					indices.push_back(ct++);
			}
		}

		static void makeHorizontalLines(int count, std::vector<QVector3D>& points, std::vector<GLushort>& indices)
		{
			auto max = std::max_element(indices.begin(), indices.end());
			int ct = *max + 1;
			for (int i = 0; i < count; ++i)
			{
				points.push_back(X + (Y * i / count)); //left point +Y 
				points.push_back(-(1.0f * X) + (Y * i / count)); //right point +Y
				points.push_back(X + ((-1.0f * Y) * i / count)); //left point -Y
				points.push_back((-1.0f * X) + ((-1.0f * Y) * i / count)); //left point -Y
				for (int i = 0; i < 4; ++i)
					indices.push_back(ct++);
			}
		}

		static void makeGridLines(int count, std::vector<QVector3D>& points, std::vector<GLushort>& indices)
		{
			makeQuadOutline(points, indices);
			makeVerticalLines(count, points, indices);
			makeHorizontalLines(count, points, indices);
		}


		static void createGrid(int count, VertexData** vertices, int& vsize, GLushort** indices, int& isize)
		{
			std::vector<QVector3D> vVect;
			std::vector<GLushort> iVect;
			makeGridLines(count, vVect, iVect);

			VertexData* vdata = new VertexData[vVect.size()];
			int ct = 0;
			for (auto point : vVect)
				vdata[ct++].position = point;

			*vertices = vdata;

			GLushort* idata = new GLushort[iVect.size()];
			ct = 0;
			for (auto index : iVect)
				idata[ct++] = index;

			*indices = idata;

			vsize = vVect.size();
			isize = iVect.size();
		}
	}

	namespace Quad
	{
		static void createQuad(VertexData** vertices, int& vsize, GLushort** indices, int& isize)
		{
			VertexData* vdata = new VertexData[QuadTemplate.size()];
			int ct = 0;
			for (auto point : QuadTemplate)
				vdata[ct++] = point;

			*vertices = vdata;

			GLushort* idata = new GLushort[QuadIndices.size()];
			ct = 0;
			for (auto index : QuadIndices)
				idata[ct++] = index;

			*indices = idata;

			vsize = QuadTemplate.size();
			isize = QuadIndices.size();
		}
	}
};
