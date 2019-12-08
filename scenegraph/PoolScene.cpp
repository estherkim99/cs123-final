#include "PoolScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"
#include <glm.hpp>
#include "CS123SceneData.h"
#include <list>
#include "SupportCanvas2D.h"

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
    m_ball_velocities.clear();

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
            m_ball_velocities.push_back(glm::vec3(0.0f));
        }
        m_ball_velocities[0] = glm::vec3(0.2f,0.f,0.4f);
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
    if(m_balls.size() == 0){
        init();
    }

    if(m_sceneObjects.size() == 38){
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
//            glm::mat4 transform = glm::translate(m_ball_translations.at(i-22)) * o.composite;
//            drawObject(o,transform);
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

void PoolScene::addVelocity(int ballNum, glm::vec3 vel){
    assert(m_ball_translations.size() == 16);
    m_ball_translations.at(ballNum) += vel;
}

void PoolScene::checkIntersections(){
    // may need to hard code initial positions of each ball D:
    for(int i = 0; i < m_ball_translations.size(); i++){
        // TODO: if the velocity is 0, dont need to check anything

        // check intersection with walls
        for(int j = 0; i < m_walls.size(); j++){
            // TODO: if there is intersection, update the velocities properly
        }

        // check intersection with holes
        for(int j = 0; i < m_holes.size(); j++){
            // TODO: if there is intersection, update the velocities properly
        }

        // check intersection with other balls
        for(int j = i+1; i < m_ball_translations.size(); j++){
            // TODO: if there is intersection, update the velocities properly
        }
    }
}

void PoolScene::updateTranslation(float secondsPassed) {
    float a = .05; // m/s^2

    if(m_ball_translations.size() == 16){
        for(int i = 0; i < m_ball_velocities.size(); i++){
            if(glm::length(m_ball_velocities[i]) < 0.0001f){
                continue;
            }
            glm::vec3 inv_norm = -glm::normalize(m_ball_velocities[i]);
            glm::vec3 new_vel = m_ball_velocities[i] + secondsPassed*a*inv_norm;
            if(signbit(new_vel.x) != signbit(m_ball_velocities[i].x)){
                new_vel.x = 0.f;
            }
            if(signbit(new_vel.z) != signbit(m_ball_velocities[i].z)){
                new_vel.z = 0.f;
            }
            m_ball_velocities[i] = new_vel;
        }

        for(int i = 0; i < m_ball_translations.size(); i++){
            m_ball_translations[i] += secondsPassed*m_ball_velocities[i];
        }
    }
}

