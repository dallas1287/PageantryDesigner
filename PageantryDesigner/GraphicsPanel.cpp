#include "graphicsPanel.h"
#include "TopWindow.h"
#include <QVector2D>
#include <QVector3D>
#include <QPainter>
#include <QWheelEvent>
#include <QtMath>
#include "utils.h"
#include <fstream>

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
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	setFocus(Qt::FocusReason::OtherFocusReason);
}

GraphicsPanel::~GraphicsPanel()
{
};

void GraphicsPanel::importModel(const QString& importPath)
{
	m_MeshRenderer.reset(new MeshRenderer(this, importPath));
}

void GraphicsPanel::initializeGL()
{
	initializeOpenGLFunctions();
	setBackground(background);
	m_floorRenderer.reset(new FloorRenderer(this));
	//m_MeshRenderer.reset(new MeshRenderer(this, "../N&I_rig_baked.fbx"));
	//m_MeshRenderer = new MeshRenderer(this, "../modeltest7_multi.fbx");
	//m_MeshRenderer.reset(new MeshRenderer(this, "../cylinderTest2.fbx"));
	//m_MeshRenderer.reset(new MeshRenderer(this, "../kitty_new6.fbx"));
	//m_MeshRenderer.reset(new MeshRenderer(this, "../cube_texture_scene.fbx"));
	//m_MeshRenderer.reset(new MeshRenderer(this, "../cube_color.fbx"));
	//m_MeshRenderer.reset(new MeshRenderer(this, "../color_sphere_uv.fbx"));
	//m_MeshRenderer.reset(new MeshRenderer(this, "../sphere_texture.fbx"));
	//m_MeshRenderer->initTextures("../cube_paint.png");
	//m_MeshRenderer->initTextures("../paint_sphere.png");
	//m_MeshRenderer->getMeshManager()->getMeshes()[0]->initTexture("../container2.png");
	//m_MeshRenderer->getMeshManager()->getMeshes()[0]->initSpecularTexture("../container2_specular.png");
	m_MeshRenderer.reset(new MeshRenderer(this));

	m_MeshRenderer->createCube(3);
	for (auto pObj : m_MeshRenderer->PrimitiveObjects())
	{
		pObj->setMeshColor(QVector4D(.992, .996, .435, 1.0));
		pObj->applyMeshColor();
		pObj->initTexture("../container2.png");
		pObj->initSpecularTexture("../container2_specular.png");
	}
	m_MeshRenderer->createQuad();
	populateAnimCb();
	populateMeshesCb();

	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);
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

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	QMatrix4x4 model;

	//m_floorRenderer->setMVP(model, m_camera.View(), m_camera.Perspective());
	//m_floorRenderer->Draw();

	m_MeshRenderer->setMVP(model, m_camera.View(), m_camera.Perspective());

	/*if (m_frame % 1 == 0)
	{
		m_MeshRenderer->getMeshManager()->animate();
		updateFrameCt(m_MeshRenderer->getMeshManager()->getFrameCt());
	}
	m_MeshRenderer->Draw();*/
	m_MeshRenderer->PrimitiveObjects()[0]->setMVP(model, m_camera.View(), m_camera.Perspective());
	model.translate(QVector3D(3.0, 0.0, 0.0));
	m_MeshRenderer->PrimitiveObjects()[1]->setMVP(model, m_camera.View(), m_camera.Perspective());
	model.setToIdentity();
	model.translate(QVector3D(-3.0, 0.0, 0.0));
	m_MeshRenderer->PrimitiveObjects()[2]->setMVP(model, m_camera.View(), m_camera.Perspective());
	/*for (auto mesh : m_MeshRenderer->PrimitiveObjects())
	{
		mesh->ShaderProgram()->bind();
		mesh->ShaderProgram()->setUniformValue("viewPos", m_camera.Position());

		mesh->ShaderProgram()->setUniformValue("light.position", m_camera.Position());
		mesh->ShaderProgram()->setUniformValue("light.direction", m_camera.Front());
		mesh->ShaderProgram()->setUniformValue("light.ambient", .2, .2, .2);
		mesh->ShaderProgram()->setUniformValue("light.diffuse", .8, .8, .8);
		mesh->ShaderProgram()->setUniformValue("light.specular", 1.0, 1.0, 1.0);
		mesh->ShaderProgram()->setUniformValue("light.constant", 1.0f);
		mesh->ShaderProgram()->setUniformValue("light.linear", 0.09f);
		mesh->ShaderProgram()->setUniformValue("light.quadratic", 0.032f);
		mesh->ShaderProgram()->setUniformValue("light.cutoff", (float)qCos(qDegreesToRadians(12.5)));
		mesh->ShaderProgram()->setUniformValue("light.outerCutoff", (float)qCos(qDegreesToRadians(17.5)));

		//mesh->ShaderProgram()->setUniformValue("material.ambient", 1.0, 0.5, 0.31);
		//mesh->ShaderProgram()->setUniformValue("material.diffuse", 1.0, 0.5, 0.31);
		//mesh->ShaderProgram()->setUniformValue("material.specular", 0.8, 0.8, 0.8);
		mesh->ShaderProgram()->setUniformValue("material.shininess", 64.0f);
		mesh->ShaderProgram()->release();
	}*/

	for (auto mesh : m_MeshRenderer->PrimitiveObjects())
	{
		mesh->ShaderProgram()->bind();
		mesh->ShaderProgram()->setUniformValue("material.shininess", 64.0f);
		mesh->ShaderProgram()->setUniformValue("viewPos", m_camera.Position());
		mesh->ShaderProgram()->setUniformValue("dirLight.direction", 2.0, -2.0, 2.0);
		mesh->ShaderProgram()->setUniformValue("dirLight.ambient", .1, .1, .8);
		mesh->ShaderProgram()->setUniformValue("dirLight.diffuse", .1, .1, .8);
		mesh->ShaderProgram()->setUniformValue("dirLight.specular", .5, .5, .5);

		mesh->ShaderProgram()->setUniformValue("pointLights[0].position", 0.0, -3.0, 0.0);
		mesh->ShaderProgram()->setUniformValue("pointLights[0].ambient", .05, .05, .05);
		mesh->ShaderProgram()->setUniformValue("pointLights[0].diffuse", 1.0, .09, .5);
		mesh->ShaderProgram()->setUniformValue("pointLights[0].specular", 1.0, 1.0, 1.0);
		mesh->ShaderProgram()->setUniformValue("pointLights[0].constant", 1.0f);
		mesh->ShaderProgram()->setUniformValue("pointLights[0].linear", 0.09f);
		mesh->ShaderProgram()->setUniformValue("pointLights[0].quadratic", 0.032f);

		mesh->ShaderProgram()->setUniformValue("pointLights[1].position", 0.0, -3.0, 0.0);
		mesh->ShaderProgram()->setUniformValue("pointLights[1].ambient", .05, .05, .05);
		mesh->ShaderProgram()->setUniformValue("pointLights[1].diffuse", 1.0, .09, .5);
		mesh->ShaderProgram()->setUniformValue("pointLights[1].specular", 1.0, 1.0, 1.0);
		mesh->ShaderProgram()->setUniformValue("pointLights[1].constant", 1.0f);
		mesh->ShaderProgram()->setUniformValue("pointLights[1].linear", 0.09f);
		mesh->ShaderProgram()->setUniformValue("pointLights[1].quadratic", 0.032f);

		mesh->ShaderProgram()->setUniformValue("pointLights[2].position", 0.0, -3.0, 0.0);
		mesh->ShaderProgram()->setUniformValue("pointLights[2].ambient", .05, .05, .05);
		mesh->ShaderProgram()->setUniformValue("pointLights[2].diffuse", 1.0, .09, .5);
		mesh->ShaderProgram()->setUniformValue("pointLights[2].specular", 1.0, 1.0, 1.0);
		mesh->ShaderProgram()->setUniformValue("pointLights[2].constant", 1.0f);
		mesh->ShaderProgram()->setUniformValue("pointLights[2].linear", 0.09f);
		mesh->ShaderProgram()->setUniformValue("pointLights[2].quadratic", 0.032f);

		mesh->ShaderProgram()->setUniformValue("pointLights[3].position", 0.0, -3.0, 0.0);
		mesh->ShaderProgram()->setUniformValue("pointLights[3].ambient", .05, .05, .05);
		mesh->ShaderProgram()->setUniformValue("pointLights[3].diffuse", 1.0, .09, .5);
		mesh->ShaderProgram()->setUniformValue("pointLights[3].specular", 1.0, 1.0, 1.0);
		mesh->ShaderProgram()->setUniformValue("pointLights[3].constant", 1.0f);
		mesh->ShaderProgram()->setUniformValue("pointLights[3].linear", 0.09f);
		mesh->ShaderProgram()->setUniformValue("pointLights[3].quadratic", 0.032f);

		mesh->ShaderProgram()->setUniformValue("spotLight.position", m_camera.Position());
		mesh->ShaderProgram()->setUniformValue("spotLight.direction", m_camera.Front());
		mesh->ShaderProgram()->setUniformValue("spotLight.ambient", 0.0, 0.0, 0.0);
		mesh->ShaderProgram()->setUniformValue("spotLight.diffuse", 0.0, 1.0, 0.0);
		mesh->ShaderProgram()->setUniformValue("spotLight.specular", 1.0, 1.0, 1.0);
		mesh->ShaderProgram()->setUniformValue("spotLight.constant", 1.0f);
		mesh->ShaderProgram()->setUniformValue("spotLight.linear", 0.09f);
		mesh->ShaderProgram()->setUniformValue("spotLight.quadratic", 0.032f);
		mesh->ShaderProgram()->setUniformValue("spotLight.cutOff", (float)qCos(qDegreesToRadians(12.5)));
		mesh->ShaderProgram()->setUniformValue("spotLight.outerCutOff", (float)qCos(qDegreesToRadians(15.0)));

		mesh->ShaderProgram()->release();
	}

	m_MeshRenderer->Draw();

	/*glActiveTexture(GL_TEXTURE3);
	mesh->Texture()->bind(GL_TEXTURE3);

	//glActiveTexture(GL_TEXTURE0);
	//mesh->SpecularTexture()->bind();

	mesh->ShaderProgram()->setUniformValue("material.diffuse", GL_TEXTURE3);
	mesh->ShaderProgram()->setUniformValue("material.specular", GL_TEXTURE0);

	mesh->bindToDraw();
	glDrawElements(GL_TRIANGLES, mesh->getIndices().size(), GL_UNSIGNED_SHORT, 0);
	mesh->releaseFromDraw();*/

	++m_frame;	
	painter.end();
	updateCameraStats();
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
	if (event->angleDelta().y() > 0)
		m_camera.zoomIn();
	else if (event->angleDelta().y() < 0)
		m_camera.zoomOut();
}

void GraphicsPanel::mouseMoveEvent(QMouseEvent* event)
{
	if (m_middlePressed)
	{
		if(abs(event->pos().x() - m_lastPos.x()) > abs(event->pos().y() - m_lastPos.y()))
			m_camera.rotateCam(event->pos().x() > m_lastPos.x() ? Direction::Left : Direction::Right);
		else
			m_camera.rotateCam(event->pos().y() > m_lastPos.y() ? Direction::Up : Direction::Down);
	}
}

void GraphicsPanel::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MouseButton::MiddleButton)
	{
		m_middlePressed = true;
		m_lastPos = event->pos();
	}
}

void GraphicsPanel::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MouseButton::MiddleButton)
		m_middlePressed = false;
}

void GraphicsPanel::keyPressEvent(QKeyEvent* event)
{
	bool shiftPress = false;
	bool ctrlPress = false;
	if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
		shiftPress = true;
	if (QGuiApplication::keyboardModifiers() & Qt::ControlModifier)
		ctrlPress = true;
	switch (event->key())
	{
	case Qt::Key_Up:
		if (shiftPress)
			m_camera.rotateCam(Direction::Up);
		else if (ctrlPress)
			m_camera.moveCam(Direction::Up);
		else
			m_camera.moveCam(Direction::Forward);
		break;
	case Qt::Key_Down:
		if (shiftPress)
			m_camera.rotateCam(Direction::Down);
		else if(ctrlPress)
			m_camera.moveCam(Direction::Down);
		else
			m_camera.moveCam(Direction::Backwards);
		break;
	case Qt::Key_Left:
		shiftPress ? m_camera.rotateCam(Direction::Left) :
		m_camera.moveCam(Direction::Left);
		break;
	case Qt::Key_Right:
		shiftPress ? m_camera.rotateCam(Direction::Right) :
		m_camera.moveCam(Direction::Right);
		break;
	case Qt::Key_A:
		m_camera.rollCam(Direction::Rotation::CCW);
		break;
	case Qt::Key_D:
		m_camera.rollCam(Direction::Rotation::CW);
		break;
	case Qt::Key_Space:
		m_camera.resetView();
		break;
	case Qt::Key_1:
		m_camera.moveCamPlane(Direction::Plane::East);
		break;
	case Qt::Key_2:
		m_camera.moveCamPlane(Direction::Plane::West);
		break;
	case Qt::Key_3:
		m_camera.moveCamPlane(Direction::Plane::Top);
		break;
	case Qt::Key_4:
		m_camera.moveCamPlane(Direction::Plane::Bottom);
		break;
	case Qt::Key_5:
		m_camera.moveCamPlane(Direction::Plane::Front);
		break;
	case Qt::Key_6:
		m_camera.moveCamPlane(Direction::Plane::Back);
		break;
	case Qt::Key_Equal:
		m_MeshRenderer->getMeshManager()->incrementFrame();
		break;
	case Qt::Key_Minus:
		m_MeshRenderer->getMeshManager()->decrementFrame();
		break;
	default:
		return;
	}
}

void GraphicsPanel::keyReleaseEvent(QKeyEvent* event)
{

	switch (event->key())
	{
	case Qt::Key_Down:
		break;
	case Qt::Key_Up:
		break;
	case Qt::Key_Left:
		break;
	case Qt::Key_Right:
		break;
	default:
		return;
	}
}

void GraphicsPanel::updateFrameCt(int value)
{
	((TopWindow*)m_parent)->updateFrameCt(value);
}

void GraphicsPanel::updateCameraStats()
{
	((TopWindow*)m_parent)->updateCameraStats();
}

void GraphicsPanel::onAnimCbChanged(int index)
{
	m_MeshRenderer->getMeshManager()->setCurrentAnimation(index);
}

void GraphicsPanel::populateAnimCb()
{
	if (!m_MeshRenderer.get())
		return;

	std::vector<QString> names;
	for (auto anim : m_MeshRenderer->getMeshManager()->getAnimations())
		names.push_back(anim->getName());
	((TopWindow*)m_parent)->populateAnimCb(names);
}

void GraphicsPanel::populateMeshesCb()
{
	if (!m_MeshRenderer.get())
		return;

	std::vector<QString> names;
	for (auto mesh : m_MeshRenderer->getMeshManager()->getMeshes())
		names.push_back(mesh->getName());
	((TopWindow*)m_parent)->populateMeshesCb(names);
}

void GraphicsPanel::setAnimationFrame(int value)
{
	if (value > 100 || value < 0)
		return;

	float frame = value * m_MeshRenderer->getMeshManager()->getCurrentAnimation()->getDuration() / 100;

	int final = qRound(frame);
	m_MeshRenderer->getMeshManager()->setFrameCt(final);
}