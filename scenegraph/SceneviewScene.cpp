#include "SceneviewScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"
#include "gl/textures/Texture2D.h"

using namespace CS123::GL;

SceneviewScene::SceneviewScene()
{
    // TODO: [SCENEVIEW] Set up anything you need for your Sceneview scene here...
    loadPhongShader();
    loadWireframeShader();
    loadNormalsShader();
    loadNormalsArrowShader();
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

void SceneviewScene::render(SupportCanvas3D *context) {
    setClearColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_phongShader->bind();
    setSceneUniforms(context);
    setLights();
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

void SceneviewScene::setLights()
{
    for (CS123SceneLightData light : m_lightData) {
        m_phongShader->setLight(light);
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
            m_cube->drawShape();
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            m_cylinder->drawShape();
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            m_cone->drawShape();
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            m_sphere->drawShape();
            break;
        case PrimitiveType::PRIMITIVE_MESH:
            break;
        case PrimitiveType::PRIMITIVE_TORUS :
            break;
        }
    }
}

void SceneviewScene::applyTextureIfUsed(SceneObject obj) {
    CS123SceneFileMap map = obj.material.textureMap;
    if (!map.isUsed) {
        m_phongShader->setUniform("useTexture", 0);
        return;
    }
    m_phongShader->setUniform("useTexture", 1);
    m_phongShader->setUniform("repeatUV", glm::vec2(map.repeatU, map.repeatV));

    QImage image = m_textures.at(obj.id);
    m_phongShader->setUniform("width", image.width());
    m_phongShader->setUniform("height", image.height());
    QImage fImage = QGLWidget::convertToGLFormat(image);

    Texture2D texture = Texture2D(fImage.bits(), fImage.width(), fImage.height());

    GLuint texturaID[1];
    glGenTextures(1, texturaID);
    glBindTexture(GL_TEXTURE_2D, texturaID[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fImage.width(), fImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, fImage.bits());

    GLint baseImageLoc = glGetUniformLocation(1, "tex"); // 1 should be m_programID; not sure how to access this here
    glUniform1i(baseImageLoc, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, texturaID[0]);
}

void SceneviewScene::tesselateShapes(){
    m_cube = std::make_unique<Cube>();
    m_cube->createVertices(settings.shapeParameter1, settings.shapeParameter2);
    m_cone = std::make_unique<Cone>();
    m_cone->createVertices(settings.shapeParameter1, settings.shapeParameter2);
    m_cylinder = std::make_unique<Cylinder>();
    m_cylinder->createVertices(settings.shapeParameter1, settings.shapeParameter2);
    m_sphere = std::make_unique<Sphere>();
    m_sphere->createVertices(settings.shapeParameter1, settings.shapeParameter2);
}

void SceneviewScene::settingsChanged() {
}

