#include "graphicsPanel.h"
#include "TopWindow.h"
#include <QVector2D>
#include <QVector3D>
#include <QPainter>
#include <QWheelEvent>

QVector4D background = { (float)135 / 255, (float)206 / 255, (float)250 / 255, 1.0f };

GraphicsPanel::GraphicsPanel(QWidget* parent, Qt::WindowFlags flags) : QOpenGLWidget(parent, flags), m_ebo(QOpenGLBuffer::IndexBuffer), m_parent(parent)
{
	/*QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setMajorVersion(4);
	format.setMinorVersion(3);
	format.setSamples(4);
	format.setProfile(QSurfaceFormat::CoreProfile);
	setFormat(format);*/
}

GraphicsPanel::~GraphicsPanel()
{
	makeCurrent();

	delete m_texture;
	delete m_program;
	delete[] m_gridData;
	delete[] m_gridIndices;

	delete m_floorRenderer;

	m_vbo.destroy();
	m_vao.destroy();

	doneCurrent();
};

void GraphicsPanel::initializeGL()
{
	initializeOpenGLFunctions();
	setBackground(background);
	initShaders();
	initTexture("../tattoo_comic.jpg");
	setupAttributes();
	m_floorRenderer = new FloorRenderer(this);
	createRectWithIndices();
}

void GraphicsPanel::setBackground(QVector4D background)
{
	glClearColor(background.x(), background.y(), background.z(), background.w());
}

void GraphicsPanel::initTexture(const QString& path)
{
	//OpenGL flips the image so it needs to be mirrored when created
	QImage image(path);
	m_texture = new QOpenGLTexture(image.mirrored());

	// Set nearest filtering mode for texture minification
	m_texture->setMinificationFilter(QOpenGLTexture::Nearest);

	// Set bilinear filtering mode for texture magnification
	m_texture->setMagnificationFilter(QOpenGLTexture::Linear);

	// Wrap texture coordinates by repeating
	m_texture->setWrapMode(QOpenGLTexture::Repeat);
}

void GraphicsPanel::initShaders()
{
	m_program = new QOpenGLShaderProgram(this);
	m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "vertex.glsl");
	m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "fragment.glsl");

	if (!m_program->link())
		close();

	
	m_gridProgram = new QOpenGLShaderProgram(this);
	m_gridProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, "grid_vertex.glsl");
	m_gridProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, "grid_fragment.glsl");

	if (!m_gridProgram->link())
		close();

	//i still am not sure when this becomes useful and necessary
	if (!m_vao.isCreated())
		m_vao.create();
}

//this works in conjunction with drawAttributes
void GraphicsPanel::setupAttributes()
{
	m_posAttr = m_program->attributeLocation("posAttr");
	m_texCoordAttr = m_program->attributeLocation("texCoordAttr");
	m_colAttr = m_program->attributeLocation("colAttr");
	m_matrixUniform = m_program->uniformLocation("matrix");

	m_gridposAttr = m_gridProgram->attributeLocation("posAttr");
	m_gridcolAttr = m_gridProgram->attributeLocation("colAttr");
	m_gridmatrixUniform = m_gridProgram->uniformLocation("matrix");
}

void GraphicsPanel::myPaint()
{
	QPainter painter;
	painter.begin(this);
	painter.beginNativePainting();

	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	QMatrix4x4 matrix;
	matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	matrix.translate(0, 0, -2);
	//matrix.rotate(m_frame, 0, 1, 0);
	
	m_floorRenderer->setMatrix(matrix);
	m_floorRenderer->Draw();
	
	painter.end();

	++m_frame;
	if (m_frame > 360)
		m_frame = 0;

	if (!m_paused)
		update();
}

void GraphicsPanel::drawLines()
{
	drawLinesWithIndices();
	//drawLinesWithBuffers();
}

void GraphicsPanel::drawRectangle()
{
	createRectWithIndices();
	//drawRectWithBuffers();
}

void GraphicsPanel::createRectWithIndices()
{
	VertexData vertices[] =
	{
		{ QVector3D(1.0f,  1.0f, 0.0f), QVector2D(1.0f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f) }, // top right
		{ QVector3D(1.0f, -1.0f, 0.0f), QVector2D(1.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f) }, // bottom right
		{ QVector3D(-1.0f, -1.0f, 0.0f), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f) }, // bottom left
		{ QVector3D(-1.0f,  1.0f, 0.0f), QVector2D(0.0f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f) }  // top left 
	};

	GLushort indices[] =
	{
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	if (!m_vbo.isCreated())
		m_vbo.create();
	m_vbo.bind();
	m_vbo.allocate(vertices, sizeof(vertices));

	m_program->enableAttributeArray(m_posAttr);
	m_program->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 3, sizeof(VertexData));

	m_texture->bind();
	m_program->enableAttributeArray(m_texCoordAttr);
	m_program->setAttributeBuffer(m_texCoordAttr, GL_FLOAT, sizeof(decltype(vertices->m_position)), 2, sizeof(VertexData));

	m_program->enableAttributeArray(m_colAttr);
	m_program->setAttributeBuffer(m_colAttr, GL_FLOAT, sizeof(decltype(vertices->m_position)) + sizeof(decltype(vertices->m_texCoord)), 3, sizeof(VertexData));
	m_vbo.release();

	if (!m_ebo.isCreated())
		m_ebo.create();
	m_ebo.allocate(indices, sizeof(indices));
	m_ebo.bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

void GraphicsPanel::drawLinesWithIndices()
{
	/*VertexData vertices[] =
	{
		{ QVector3D(0.0f, 0.0f, 0.0f), QVector2D(), QVector3D() },
		{ QVector3D(0.75f, 0.75f, 0.0f), QVector2D(), QVector3D() },
		{ QVector3D(-0.75f, 0.75f, 0.0f), QVector2D(), QVector3D() }
	};

	GLushort indices[] =
	{
		0, 1,
		0, 2,
	};*/

	VertexData* verts;
	GLushort* indices;
	int vSize, iSize;

	VertexData::makeGrid(4, &verts, vSize, &indices, iSize);

	glLineWidth(5.0);
	
	if (!m_vbo.isCreated())
		m_vbo.create();
	m_vbo.bind();

	m_vbo.allocate(verts, vSize * sizeof(VertexData));
	m_gridProgram->enableAttributeArray(m_posAttr);
	m_gridProgram->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 3, sizeof(VertexData));

	m_vbo.release();

	if (!m_ebo.isCreated())
		m_ebo.create();
	m_ebo.allocate(indices, iSize * sizeof(GLushort));
	m_ebo.bind();

	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, 0);

	glLineWidth(2.0);

	glDrawElements(GL_LINES, iSize, GL_UNSIGNED_SHORT, 0);

	delete[] verts;
	delete[] indices;
}

void GraphicsPanel::paintGL()
{
	myPaint();
}

void GraphicsPanel::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}

void GraphicsPanel::onPlayClicked()
{
	m_paused ^= true;
	if (!m_paused)
		myPaint();
}

void GraphicsPanel::wheelEvent(QWheelEvent* event)
{
	qDebug() << "Mouse wheel detected" << event->angleDelta();
}

void GraphicsPanel::cleanup()
{
	makeCurrent();
	delete m_texture;
	m_texture = 0;
	delete m_program;
	doneCurrent();
}