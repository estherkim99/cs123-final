#include "PoolScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"
#include "gl/textures/Texture2D.h"
#include "gl/textures/TextureParametersBuilder.h"


using namespace CS123::GL;


PoolScene::PoolScene() : SceneviewScene()
{
    // TODO: [SCENEVIEW] Set up anything you need for your Sceneview scene here...
    init();
}

PoolScene::~PoolScene()
{
}

void PoolScene::init(){
    m_object_rotations.clear();
    m_object_translations.clear();
    m_ball_translations.clear();
    m_walls.clear();
    m_holes.clear();
    m_balls.clear();

    if(m_sceneObjects.size() == 38){
        for(int i = 0; i <= 15; i++){
            m_walls.push_back(m_sceneObjects.at(i));
        }
        for(int i = 16; i <= 21; i++){
            m_holes.push_back(m_sceneObjects.at(i));
        }
        for(int i = 22; i <= 37; i++){
            m_balls.push_back(m_sceneObjects.at(i));
            m_ball_translations.push_back(glm::vec3(0.0f));
        }
    }
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

void PoolScene::renderGeometry() {
    if (m_balls.size() == 0){
        init();
    }

    if (m_sceneObjects.size() == 38){ // make this more extensible?
        for(int i = 0; i <= 15; i++){
            SceneObject o = m_walls.at(i);
            glm::mat4 transform =  o.composite;
            drawObject(o,transform);
        }
        for(int i = 16; i <= 21; i++){
            SceneObject o = m_holes.at(i-16);
            glm::mat4 transform =  o.composite;
            drawObject(o,transform);
        }
        for(int i = 22; i <= 37; i++){
            SceneObject o = m_balls.at(i-22);
            glm::mat4 transform = glm::translate(m_ball_translations.at(i-22)) * o.composite;
            drawObject(o,transform);
        }
    }
}

void PoolScene::drawObject(SceneObject o, glm::mat4 transform){
    o.material.cAmbient *= m_ka;
    o.material.cDiffuse *= m_kd;
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

void PoolScene::updateBallPosition(int ballNum, glm::vec3 translate){
    assert(0 <= ballNum && ballNum <= 17);
    m_ball_rotations.at(ballNum) += translate;
}

// This is just a filler method right now. We can change it to alter the
// objects in any way we need
void PoolScene::updateTranslation() {
    for(int i = 0; i < m_ball_translations.size(); i++){
        glm::vec3 translate = glm::vec3(0.01f,0.f,0.f);
        m_ball_translations[i] += translate;
    }
}

