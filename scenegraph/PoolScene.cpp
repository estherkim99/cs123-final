#include "PoolScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"



using namespace CS123::GL;


PoolScene::PoolScene()
{
    // TODO: [SCENEVIEW] Set up anything you need for your Sceneview scene here...
    loadPhongShader();
    loadWireframeShader();
    loadNormalsShader();
    loadNormalsArrowShader();
    tesselateShapes();

    init();
}

PoolScene::~PoolScene()
{
}

void PoolScene::init(){
    m_object_rotations.clear();
    m_object_translations.clear();
    for(int i = 0; i < m_sceneObjects.size(); i++){
            m_object_translations.push_back(glm::vec3(0.0f));
            m_object_rotations.push_back(glm::vec3(0.0f));
    }
}

void PoolScene::loadPhongShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/default.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void PoolScene::loadWireframeShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.frag");
    m_wireframeShader = std::make_unique<Shader>(vertexSource, fragmentSource);
}

void PoolScene::loadNormalsShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normals.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.frag");
    m_normalsShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void PoolScene::loadNormalsArrowShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.frag");
    m_normalsArrowShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void PoolScene::render(SupportCanvas3D *context) {
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

void PoolScene::setSceneUniforms(SupportCanvas3D *context) {
    Camera *camera = context->getCamera();
    m_phongShader->setUniform("useLighting", settings.useLighting);
    m_phongShader->setUniform("useArrowOffsets", false);
    m_phongShader->setUniform("p" , camera->getProjectionMatrix());
    m_phongShader->setUniform("v", camera->getViewMatrix());
}

void PoolScene::setMatrixUniforms(Shader *shader, SupportCanvas3D *context) {
    shader->setUniform("p", context->getCamera()->getProjectionMatrix());
    shader->setUniform("v", context->getCamera()->getViewMatrix());
}

void PoolScene::setLights()
{
    for (CS123SceneLightData light : m_lightData) {
        m_phongShader->setLight(light);
    }
}

void PoolScene::renderGeometry() {
    if(m_object_translations.size() == 0){
        init();
    }

    for (int i = 0; i < m_sceneObjects.size(); i++) {
        SceneObject o = m_sceneObjects[i];
        o.material.cAmbient *= m_ka;
        o.material.cDiffuse *= m_kd;
        glm::mat4 transform = glm::translate(m_object_translations.at(i)) * o.composite;
        m_phongShader->applyMaterial(o.material);
        m_phongShader->setUniform("m", transform);
        if (settings.drawNormals) {
            m_normalsShader->setUniform("m", transform);
            m_normalsArrowShader->setUniform("m", transform);
        }
        if (settings.drawWireframe) {
            m_wireframeShader->setUniform("m", transform);
        }

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

void PoolScene::tesselateShapes(){
    m_cube = std::make_unique<CubeShape>();
    m_cone = std::make_unique<ConeShape>();
    m_cylinder = std::make_unique<CylinderShape>();
    m_sphere = std::make_unique<SphereShape>();
    m_cube->initializeShape(3, 3);
    m_cone->initializeShape(30, 30);
    m_cylinder->initializeShape(30, 30);
    m_sphere->initializeShape(30, 30);
}

void PoolScene::settingsChanged() {
}

// This is just a filler method right now. We can change it to alter the
// objects in any way we need
void PoolScene::updateTranslation() {
    for(int i = 0; i < m_object_translations.size(); i++){
        glm::vec3 translate = glm::vec3(0.01f,0.f,0.f);
        m_object_translations[i] += translate;
    }
}

