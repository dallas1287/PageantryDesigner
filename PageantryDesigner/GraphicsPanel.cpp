#include "graphicsPanel.h"
#include "TopWindow.h"
#include <QVector2D>
#include <QVector3D>
#include <QPainter>
#include <QWheelEvent>
#include "ShapeCreator.h"

const QVector4D background = { (float)135 / 255, (float)206 / 255, (float)250 / 255, 1.0f };

GraphicsPanel::GraphicsPanel(QWidget* parent, Qt::WindowFlags flags) : QOpenGLWidget(parent, flags), m_parent(parent)
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

	delete m_floorRenderer;
	delete m_dotsRenderer;

	doneCurrent();
};

void GraphicsPanel::initializeGL()
{
	initializeOpenGLFunctions();
	setBackground(background);
	m_floorRenderer = new FloorRenderer(this);
	m_dotsRenderer = new DotsRenderer(this);

}

void GraphicsPanel::setBackground(QVector4D background)
{
	glClearColor(background.x(), background.y(), background.z(), background.w());
}

void GraphicsPanel::myPaint()
{
	QPainter painter;
	painter.begin(this);
	painter.beginNativePainting();

	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	QMatrix4x4 projection;
	projection.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	projection.translate(0, 0, -2);
	projection.rotate(m_frame, 0, 1, 0);
	
	QMatrix4x4 model;
	model.setToIdentity();
	QMatrix4x4 view;
	view.setToIdentity();

	m_floorRenderer->setMVP(model, view, projection);
	m_floorRenderer->Draw();

	model.scale(.05);
	model.translate(1.01 * Z);

	for (int i = 0; i < 3; ++i)
	{
		model.translate(2.0* (X + Y));
		m_dotsRenderer->setMVP(model, view, projection);
		m_dotsRenderer->Draw();
	}
	
	painter.end();

	++m_frame;
	if (m_frame > 360)
		m_frame = 0;

	if (!m_paused)
		update();
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

	doneCurrent();
}