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

static unsigned int planeVAO;
static unsigned int depthMap;
static unsigned int depthMapFBO;
void GraphicsPanel::initializeGL()
{
	initializeOpenGLFunctions();
	setBackground(background);
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
	
	/*m_MeshRenderer->createQuad();
	m_MeshRenderer->createCube(3);

	for (auto pObj : m_MeshRenderer->PrimitiveObjects())
	{
		if (pObj->getType() == Primitive::Type::Cube)
		{
			pObj->initTexture("../container2.png");
			pObj->initSpecularTexture("../container2_specular.png");
			pObj->setSceneData(USES_MATERIAL_TEXTURES | USES_LIGHTS | HAS_DIRECTIONAL_LIGHTS | HAS_POINT_LIGHTS | HAS_SPOTLIGHTS);
		}
		else if (pObj->getType() == Primitive::Type::Quad)
		{
			pObj->initTexture("../wood.png");
			pObj->setSceneData(USES_MATERIAL_TEXTURES | USES_LIGHTS | HAS_POINT_LIGHTS| HAS_DIRECTIONAL_LIGHTS  | HAS_SPOTLIGHTS);
		}
	}

	//framebuffer test initialization
	std::vector<VertexData> vdata;
	std::vector<GLushort> idata;
	ShapeMaker::Cube::createCube(vdata, idata);
	m_MeshRenderer->getShadowMap()->setVertexData(vdata);
	m_MeshRenderer->getShadowMap()->setIndices(idata);
	//m_MeshRenderer->getShadowMap()->initShaders(SHADOW_MAP_VS, SHADOW_MAP_FRAG);
	m_MeshRenderer->getShadowMap()->initShaders("mesh_vertex.glsl", "color_frag.glsl");
	m_MeshRenderer->getShadowMap()->initBuffers(vdata, idata);
	m_MeshRenderer->getShadowMap()->getQuad()->initTexture("../tattoo_comic.jpg");*/

	populateAnimCb();
	populateMeshesCb();

	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);

	float planeVertices[] = {
		// positions            // normals         // texcoords
		 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

		 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 10.0f
	};
	// plane VAO
	unsigned int planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	// load textures
	// -------------
	//unsigned int woodTexture = loadTexture(FileSystem::getPath("resources/textures/wood.png").c_str());

	// configure depth map FBO
	// -----------------------
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture	
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	//glDrawBuffer(GL_NONE);
	GLenum buf = GL_NONE;
	glDrawBuffers(1, &buf);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_MeshRenderer->getShadowMap()->initShaders("shadowMap_vs.glsl", "shadowMap_frag.glsl");
	m_MeshRenderer->getShadowMap()->initTexture("../wood.png");
	
	// shader configuration
	// --------------------
	m_MeshRenderer->getShadowMap()->getQuad()->ShaderProgram()->bind();
	m_MeshRenderer->getShadowMap()->getQuad()->ShaderProgram()->setUniformValue("depthMap", 0);
	

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
	//glClearColor(0.1, 0.1, 0.1, 1.0);

	QVector3D lightPos(-2.0f, 4.0f, -1.0);

	QMatrix4x4 lightProjection, lightView;
	QMatrix4x4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 7.5f;
	lightProjection.ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView.lookAt(lightPos, QVector3D(), QVector3D(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	// render scene from light's point of view
	m_MeshRenderer->getShadowMap()->ShaderProgram()->bind();
	m_MeshRenderer->getShadowMap()->ShaderProgram()->setUniformValue("lightSpaceMatrix", lightSpaceMatrix);

	//not a part of the original code
	m_MeshRenderer->getShadowMap()->initDepthMap();

	glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, /*woodTexture*/ m_MeshRenderer->getShadowMap()->Texture()->textureId());
	renderScene(m_MeshRenderer->getShadowMap()->ShaderProgram());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render Depth map to quad for visual debugging
	// ---------------------------------------------
	m_MeshRenderer->getShadowMap()->getQuad()->ShaderProgram()->bind();
	m_MeshRenderer->getShadowMap()->getQuad()->ShaderProgram()->setUniformValue("near_plane", near_plane);
	m_MeshRenderer->getShadowMap()->getQuad()->ShaderProgram()->setUniformValue("far_plane", far_plane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	renderQuad();



	++m_frame;	
	painter.end();
	updateCameraStats();
	update();
}

// renders the 3D scene
// --------------------
void GraphicsPanel::renderScene(QOpenGLShaderProgram* shader)
{
	// floor
	QMatrix4x4 model;
	shader->setUniformValue("model", model);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	// cubes
	model.translate(0.0f, 1.5f, 0.0);
	model.scale(0.5);
	shader->setUniformValue("model", model);
	renderCube();
	model.setToIdentity();
	model.translate(2.0f, 0.0f, 1.0);
	model.scale(0.5);
	shader->setUniformValue("model", model);
	renderCube();
	model.setToIdentity();
	model.translate(-1.0f, 0.0f, 2.0);
	model.rotate(60.0, QVector3D(1.0, 0.0, 1.0).normalized());
	model.scale(0.25);
	shader->setUniformValue("model", model);
	renderCube();
}


// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void GraphicsPanel::renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void GraphicsPanel::renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void GraphicsPanel::Animating()
{
	QMatrix4x4 model;
	m_MeshRenderer->setMVP(model, m_camera.View(), m_camera.Perspective());

	if (m_frame % 1 == 0)
	{
		m_MeshRenderer->getMeshManager()->animate();
		updateFrameCt(m_MeshRenderer->getMeshManager()->getFrameCt());
	}
	m_MeshRenderer->Draw();
}

void GraphicsPanel::FrameBufferKinda()
{
	QMatrix4x4 model;
	m_MeshRenderer->getShadowMap()->setLightSpaceMatrix(QVector3D(0.0, 3.0, 0.0));
	m_MeshRenderer->getShadowMap()->setModelUniform(model);

	m_MeshRenderer->getShadowMap()->setViewport();
	m_MeshRenderer->getShadowMap()->initFrameBuffer();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		qDebug() << "OpenGL Frambuffer status not complete.";

	m_MeshRenderer->getShadowMap()->setMVP(model, m_camera.View(), m_camera.Perspective());
	m_MeshRenderer->getShadowMap()->bindToDraw();
	m_MeshRenderer->getShadowMap()->Draw();
	m_MeshRenderer->getShadowMap()->releaseFromDraw();
	//m_MeshRenderer->getShadowMap()->saveBufferAsImage();
	model.scale(5.0);
	GraphicsObject* obj = m_MeshRenderer->getShadowMap()->getQuad();
	obj->setMVP(model, m_camera.View(), m_camera.Perspective());
	if (!obj->ShaderProgram())
		return;
	obj->ShaderProgram()->bind();

	if (obj->Texture())
	{
		glActiveTexture(GL_TEXTURE3);
		obj->Texture()->bind(GL_TEXTURE3);
		obj->ShaderProgram()->setUniformValue("tex", GL_TEXTURE3 - GL_TEXTURE0);
	}
	obj->Vao().bind();
	m_MeshRenderer->getShadowMap()->getQuad()->Draw();
	m_MeshRenderer->getShadowMap()->getQuad()->releaseFromDraw();
}

void GraphicsPanel::DrawLighting()
{
	QMatrix4x4 model;
	//model.rotate(-90.0, X);
	//model.scale(10);
	//m_MeshRenderer->PrimitiveObjects()[0]->setMVP(model, m_camera.View(), m_camera.Perspective());
	//model.setToIdentity();
	m_MeshRenderer->PrimitiveObjects()[0]->setMVP(model, m_camera.View(), m_camera.Perspective());
	model.translate(QVector3D(3.0, 0.0, 0.0));
	m_MeshRenderer->PrimitiveObjects()[1]->setMVP(model, m_camera.View(), m_camera.Perspective());
	model.setToIdentity();
	model.translate(QVector3D(-3.0, 0.0, 0.0));
	m_MeshRenderer->PrimitiveObjects()[2]->setMVP(model, m_camera.View(), m_camera.Perspective());
	model.setToIdentity();

	//tempLightSetup();
	//m_MeshRenderer->Draw();
}

void GraphicsPanel::tempLightSetup()
{
	for (auto mesh : m_MeshRenderer->PrimitiveObjects())
	{
		mesh->ShaderProgram()->bind();

		mesh->ShaderProgram()->setUniformValue("material.diffuse", .1, .1, .1);
		mesh->ShaderProgram()->setUniformValue("material.specular", 1.0, 1.0, 1.0);

		mesh->ShaderProgram()->setUniformValue("blinn", m_blinn);

		mesh->ShaderProgram()->setUniformValue("material.shininess", 64.0f);
		mesh->ShaderProgram()->setUniformValue("viewPos", m_camera.Position());
		mesh->ShaderProgram()->setUniformValue("dirLight.direction", 0.0, -3.0, 0.0);
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
		mesh->ShaderProgram()->setUniformValue("spotLight.ambient", 0.1, 0.1, 0.1);
		mesh->ShaderProgram()->setUniformValue("spotLight.diffuse", 1.0, 1.0, 1.0);
		mesh->ShaderProgram()->setUniformValue("spotLight.specular", 1.0, 1.0, 1.0);
		mesh->ShaderProgram()->setUniformValue("spotLight.constant", 1.0f);
		mesh->ShaderProgram()->setUniformValue("spotLight.linear", 0.09f);
		mesh->ShaderProgram()->setUniformValue("spotLight.quadratic", 0.032f);
		mesh->ShaderProgram()->setUniformValue("spotLight.cutOff", (float)qCos(qDegreesToRadians(12.5)));
		mesh->ShaderProgram()->setUniformValue("spotLight.outerCutOff", (float)qCos(qDegreesToRadians(17.5)));

		mesh->ShaderProgram()->release();
	}
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
	case Qt::Key_B:
		m_blinn ^= true;
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