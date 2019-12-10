#include "PoolScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"



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
//        m_ball_velocities[0] = glm::vec3(0.2f,0.f,0.4f);
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

void PoolScene::tick(float secondsPassed){
    collisionDetection();
    updateTranslation(secondsPassed);
}

void PoolScene::addVelocity(int ballNum, glm::vec3 vel){
    assert(m_ball_translations.size() == 16);
    m_ball_velocities.at(ballNum) += vel;
}

void PoolScene::collisionDetection(){
    // may need to hard code initial positions of each ball D:
    for(int i = 0; i < m_ball_translations.size(); i++){
        if(glm::length(m_ball_velocities[i]) < 0.0001f){
            continue;
        }


        // check intersection with holes
        for(int j = 0; j < m_holes.size(); j++){
            // TODO: if there is intersection, update the velocities properly
        }

        // check intersection with walls
        checkWallCollision(getBallPosition(i), i);

        // check intersection with other balls
        for(int j = i+1; j < m_ball_translations.size(); j++){
            if(checkBallCollision(getBallPosition(i),getBallPosition(j))){
                updateVelocities(i,j);
            }
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

glm::vec3 PoolScene::getBallPosition(int ballnum){
    SceneObject *cue = &m_balls.at(ballnum);

    glm::vec3 initial_position = glm::vec3(cue->composite[3][0], cue->composite[3][1], cue->composite[3][2]);
    return m_ball_translations.at(ballnum) + initial_position;
}

void PoolScene::updateVelocities(int b1, int b2){
    glm::vec3 v1 = m_ball_velocities[b1];
    glm::vec3 v2 = m_ball_velocities[b2];

    glm::vec3 p1 = getBallPosition(b1);
    glm::vec3 p2 = getBallPosition(b2);

    // currently assuming cue and number balls have same mass
    // 2 particles colliding equation:
    // https://en.wikipedia.org/wiki/Elastic_collision
    glm::vec3 new_v1 = v1 - glm::dot(v1-v2,p1-p2)*(p1-p2)/glm::pow(glm::length(p1-p2),2.f);
    glm::vec3 new_v2 = v2 - glm::dot(v2-v1,p2-p1)*(p2-p1)/glm::pow(glm::length(p2-p1),2.f);

    m_ball_velocities[b1] = new_v1;
    m_ball_velocities[b2] = new_v2;
}

bool PoolScene::checkBallCollision(glm::vec3 pos1, glm::vec3 pos2){
    float dist = std::sqrt(std::pow(pos1.x - pos2.x, 2) + std::pow(pos1.z - pos2.z, 2));
    float DIAMETER = 0.05715f - 0.0001f;
    return dist < DIAMETER;
}

bool PoolScene::checkHoleCollision(glm::vec3 pos1, glm::vec3 pos2){
    float dist = std::sqrt(std::pow(pos1.x - pos2.x, 2) + std::pow(pos1.z - pos2.z, 2));
    float DIAMETER = 0.028575f+.065f;
    return dist < DIAMETER;
}

void PoolScene::checkWallCollision(glm::vec3 pos, int ballnum){
    // lining location - cube radius * z scale - radius of ball
    float z_border = 1.1515f - 0.5f * 0.063f - 0.028575f;
    float x_border = 0.5915f - 0.5f * 0.063f - 0.028575f;
    if(pos.z >= z_border || pos.z <= -z_border){
        m_ball_velocities[ballnum].z *= -1.f;
    }
    else if(pos.x >= x_border || pos.x <= -x_border){
        m_ball_velocities[ballnum].x *= -1.f;
    }
}
