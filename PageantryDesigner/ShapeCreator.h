#pragma once
#include <QVector2D>
#include <QVector3D>

struct VertexData
{
public:
	VertexData(QVector3D position = QVector3D(), QVector2D texCoord = QVector2D(), QVector3D color = QVector3D()) : m_position(position), m_texCoord(texCoord), m_color(color)  {};
	~VertexData() {};
	QVector3D m_position;
	QVector2D m_texCoord;
	QVector3D m_color;

	static void makeGrid(int count, VertexData** vertices, int& vsize, GLushort** indices, int& isize)
	{
		std::vector<QVector3D> vVect;
		std::vector<GLushort> iVect;
		makeGridLines(count, vVect, iVect);

		VertexData* vdata = new VertexData[vVect.size()];
		int ct = 0;
		for (auto point : vVect)
			vdata[ct++].m_position = point;

		*vertices = vdata;

		GLushort* idata = new GLushort[iVect.size()];
		ct = 0;
		for (auto index : iVect)
			idata[ct++] = index;

		*indices = idata;

		vsize = vVect.size();
		isize = iVect.size();
	}


	static void makeGridLines(int count, std::vector<QVector3D>& points, std::vector<GLushort>& indices)
	{
		makeOutline(points, indices);
		makeVerticalLines(count, points, indices);
		makeHorizontalLines(count, points, indices);
	}

	static void makeOutline(std::vector<QVector3D>& points, std::vector<GLushort>& indices)
	{
		points.push_back(QVector3D(-1.0f, 1.0f, 0.0f)); //top left
		points.push_back(QVector3D(1.0f, 1.0f, 0.0f)); // top Right
		points.push_back(QVector3D(1.0f, -1.0f, 0.0f)); // bottom right
		points.push_back(QVector3D(-1.0f, -1.0f, 0.0f)); // bottom left

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
			QVector3D toppoint((1.0f * i) / count, 1.0f, 0.0f);
			QVector3D bottompoint((1.0f * i) / count, -1.0f, 0.0f);
			points.push_back(toppoint);
			points.push_back(bottompoint);
			indices.push_back(ct++);
			indices.push_back(ct++);
		}
		for (int j = 1; j < count; ++j)
		{
			QVector3D toppoint((-1.0f * j) / count, 1.0f , 0.0f);
			QVector3D bottompoint((-1.0f * j) / count, -1.0f, 0.0f);
			points.push_back(toppoint);
			points.push_back(bottompoint);
			indices.push_back(ct++);
			indices.push_back(ct++);
		}
	}

	static void makeHorizontalLines(int count, std::vector<QVector3D>& points, std::vector<GLushort>& indices)
	{
		auto max = std::max_element(indices.begin(), indices.end());
		int ct = *max + 1;
		for (int i = 0; i < count; ++i)
		{
			QVector3D toppoint(1.0f, (1.0f * i) / count, 0.0f);
			QVector3D bottompoint(-1.0f, (1.0f * i) / count, 0.0f);
			points.push_back(toppoint);
			points.push_back(bottompoint);
			indices.push_back(ct++);
			indices.push_back(ct++);
		}
		for (int j = 1; j < count; ++j)
		{
			QVector3D toppoint(1.0f, (-1.0f * j) / count, 0.0f);
			QVector3D bottompoint(-1.0f, (-1.0f * j) / count, 0.0f);
			points.push_back(toppoint);
			points.push_back(bottompoint);
			indices.push_back(ct++);
			indices.push_back(ct++);
		}
	}
	
	static void drawOutline(VertexData** data, int& vSize, GLushort** indices, int& iSize)
	{
		VertexData* vdata = new VertexData[4];

		vdata[0].m_position = QVector3D(-1.0f, 1.0f, 0.0f); //top left
		vdata[1].m_position = QVector3D(1.0f, 1.0f, 0.0f); // top Right
		vdata[2].m_position = QVector3D(1.0f, -1.0f, 0.0f); // bottom right
		vdata[3].m_position = QVector3D(-1.0f, -1.0f, 0.0f); // bottom left
		*data = vdata;

		GLushort* vindices = new GLushort[8];
		vindices[0] = 0;
		vindices[1] = 1;
		vindices[2] = 1;
		vindices[3] = 2;
		vindices[4] = 2;
		vindices[5] = 3;
		vindices[6] = 3;
		vindices[7] = 0;
		*indices = vindices;

		vSize = 4;
		iSize = 8;
	}
};
