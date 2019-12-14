#include "SceneviewScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"

#include <utility>
#include <gl/textures/Texture2D.h>
#include "gl/textures/TextureParameters.h"
#include "gl/textures/TextureParametersBuilder.h"
#include "gl/textures/RenderBuffer.h"
#include "gl/textures/DepthBuffer.h"
#include "gl/datatype/FBO.h"

#include "GL/glew.h"

#include "gl/GLDebug.h"
#include "gl/textures/RenderBuffer.h"
#include "gl/textures/DepthBuffer.h"
#include "gl/textures/Texture2D.h"
#include "gl/textures/TextureParametersBuilder.h"
#include <iostream>
#include "lib/ResourceLoader.h"
#include <QFileInfo>
#include <QDebug>

using namespace CS123::GL;

SceneviewScene::SceneviewScene() :
    m_mustLoadTextures(true),
    m_mustLoadDepths(true),
    m_depthFBO(nullptr)
{
    // TODO: [SCENEVIEW] Set up anything you need for your Sceneview scene here...
    loadPhongShader();
    loadWireframeShader();
    loadNormalsShader();
    loadNormalsArrowShader();
    loadDepthShader();
    tesselateShapes();
}

SceneviewScene::~SceneviewScene()
{
}

void SceneviewScene::loadPhongShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/default.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void SceneviewScene::loadWireframeShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.frag");
    m_wireframeShader = std::make_unique<Shader>(vertexSource, fragmentSource);
}

void SceneviewScene::loadNormalsShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normals.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.frag");
    m_normalsShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void SceneviewScene::loadNormalsArrowShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.frag");
    m_normalsArrowShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void SceneviewScene::loadDepthShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/shaders/depth/depth.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/shaders/depth/depth.frag");
    m_depthShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void SceneviewScene::render(SupportCanvas3D *context) {
    setClearColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_phongShader->bind();
    setSceneUniforms(context);
    setLights(m_phongShader.get());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    renderGeometry();
    glBindTexture(GL_TEXTURE_2D, 0);
    m_phongShader->unbind();

    if (settings.drawWireframe) {
        m_wireframeShader->bind();
        setMatrixUniforms(m_wireframeShader.get(), context);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        renderGeometry();
        m_wireframeShader->unbind();
    }

    if (settings.drawNormals) {
        // Render the lines.
        m_normalsShader->bind();
        setMatrixUniforms(m_normalsShader.get(), context);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        renderGeometry();
        m_normalsShader->unbind();

        // Render the arrows.
        m_normalsArrowShader->bind();
        setMatrixUniforms(m_normalsArrowShader.get(), context);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        renderGeometry();
        m_normalsArrowShader->unbind();
    }
}

void SceneviewScene::setSceneUniforms(SupportCanvas3D *context) {
    Camera *camera = context->getCamera();
    m_phongShader->setUniform("useLighting", settings.useLighting);
    m_phongShader->setUniform("useArrowOffsets", false);
    m_phongShader->setUniform("p" , camera->getProjectionMatrix());
    m_phongShader->setUniform("v", camera->getViewMatrix());
}

void SceneviewScene::setMatrixUniforms(Shader *shader, SupportCanvas3D *context) {
    shader->setUniform("p", context->getCamera()->getProjectionMatrix());
    shader->setUniform("v", context->getCamera()->getViewMatrix());
}

void SceneviewScene::setLights(CS123Shader *shader)
{
    for (CS123SceneLightData light : m_lightData) {
        shader->setLight(light);
    }
}

void SceneviewScene::renderGeometry() {
    for (int i = 0; i < m_sceneObjects.size(); i++) {
        SceneObject o = m_sceneObjects[i];
        o.material.cAmbient *= m_ka;
        o.material.cDiffuse *= m_kd;
        m_phongShader->applyMaterial(o.material);
        m_phongShader->setUniform("m", o.composite);
        if (settings.drawNormals) {
            m_normalsShader->setUniform("m", o.composite);
            m_normalsArrowShader->setUniform("m", o.composite);
        }
        if (settings.drawWireframe) {
            m_wireframeShader->setUniform("m", o.composite);
        }
        applyTextureIfUsed(o);

        // draw shapes
        switch (o.primitive) {
        case PrimitiveType::PRIMITIVE_CUBE :
            m_cube->draw();
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            m_cylinder->draw();
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            m_cone->draw();
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            m_sphere->draw();
            break;
        case PrimitiveType::PRIMITIVE_MESH:
            break;
        case PrimitiveType::PRIMITIVE_TORUS :
            break;
        }
    }

}

void SceneviewScene::tesselateShapes(){
    m_cube = std::make_unique<CubeShape>();
    m_cone = std::make_unique<ConeShape>();
    m_cylinder = std::make_unique<CylinderShape>();
    m_sphere = std::make_unique<SphereShape>();
    m_cube->initializeShape(3, 3);
    m_cone->initializeShape(30, 30);
    m_cylinder->initializeShape(30, 30);
    m_sphere->initializeShape(30, 30);
}

void SceneviewScene::settingsChanged() {
}

void SceneviewScene::loadDepthTextures() {
    for (int i = 0; i < m_lightData.size(); i++) {
        unsigned FBO;
        unsigned Map;

        glGenFramebuffers(1, &FBO);
        glGenTextures(1, &Map);
        glBindTexture(GL_TEXTURE_2D, Map);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Map, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        depthMapFBO.push_back(FBO);
        depthMap.push_back(Map); // shadow map
    }

    m_mustLoadDepths = false;
}

void SceneviewScene::loadTextures() {
     for (SceneObject obj : m_sceneObjects) {
         CS123SceneFileMap textureMap = obj.material.textureMap;
         if (textureMap.isUsed) {
             QImage image =  QImage(QString::fromStdString(textureMap.filename));
             QImage formattedImage = QGLWidget::convertToGLFormat(image);
             Texture2D texture(formattedImage.bits(), formattedImage.width(), formattedImage.height());
             // format texture with correct params
             TextureParametersBuilder builder;
             builder.setFilter(TextureParameters::FILTER_METHOD::LINEAR);
             builder.setWrap(TextureParameters::WRAP_METHOD::REPEAT);
             TextureParameters params = builder.build();
             params.applyTo(texture);
             // add (filename, texture) pair to hashmap
             m_textures.insert(std::make_pair(textureMap.filename, std::move(texture)));
         }
     }
     m_mustLoadTextures = false;
 }

 void SceneviewScene::applyTextureIfUsed(SceneObject obj) {
     CS123SceneFileMap map = obj.material.textureMap;
     if (!map.isUsed) {
         m_phongShader->setUniform("useTexture", 0);
         return;
     }
     m_phongShader->setUniform("useTexture", 1);
     m_phongShader->setUniform("repeatUV", glm::vec2(map.repeatU, map.repeatV));
     m_phongShader->setTexture("tex", m_textures.at(map.filename));
 }
