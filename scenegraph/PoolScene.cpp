#include "PoolScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"

#include<set> // for set operations

#include <utility>
#include <gl/textures/Texture2D.h>
#include "gl/textures/TextureParameters.h"
#include "gl/textures/TextureParametersBuilder.h"


using namespace CS123::GL;


PoolScene::PoolScene() :
    SceneviewScene()
{

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
    m_ball_done.clear();

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
            m_ball_dir.push_back(glm::vec3(0.f,0.f,1.f));
            m_ball_done.push_back(false);

            m_ball_rotations.push_back(glm::mat4(1.f));
        }
    }
}

void PoolScene::render(SupportCanvas3D *context) {
    setClearColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_mustLoadTextures) loadTextures();
    if (m_mustLoadBumpMaps) loadBumpMap();

//    m_phongShader->bind();
//    setSceneUniforms(context);
//    setLights();
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    renderGeometry();
//    glBindTexture(GL_TEXTURE_2D, 0);
//    m_phongShader->unbind();

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

        // FBO
        unsigned int depthMapFBO;
        glGenFramebuffers(1, &depthMapFBO);
        unsigned int depthMap;
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                     SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // RENDER TO DEPTH MAP

//        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            //ConfigureShaderAndMatrices();
            m_simpleDepthShader->bind();
            float near_plane = 1.0f, far_plane = 7.5f;
            glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
                                              glm::vec3( 0.0f, 0.0f,  0.0f),
                                              glm::vec3( 0.0f, 1.0f,  0.0f));
            glm::mat4 lightSpaceMatrix = lightProjection * lightView;
            m_simpleDepthShader->setUniform("lightSpaceMatrix", lightSpaceMatrix);

            renderGeometry();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_simpleDepthShader->unbind();

        // RENDER NORMAL

//        glViewport(0, 0, 1300, 1300);
//                glViewport(0, 0, context->width(), context->height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //ConfigureShaderAndMatrices();
        m_phongShader->bind();
        setSceneUniforms(context);
        setLights();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        //RenderScene();
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
            drawObject(o,transform, -1);
        }
        for(int i = 16; i <= 21; i++){
            SceneObject o = m_holes.at(i-16);
            glm::mat4 transform =  o.composite;
            drawObject(o,transform, -1);
        }
        for(int i = 22; i <= 37; i++){
            if(m_ball_done.at(i-22)){
                continue;
            }
            SceneObject o = m_balls.at(i-22);

            glm::mat4 transform = glm::translate(m_ball_translations.at(i-22))
                    * o.composite;

            transform = transform * m_ball_rotations.at(i-22);

            drawObject(o,transform, i-22);
        }
    }

}

void PoolScene::drawObject(SceneObject o, glm::mat4 transform, int i){
    o.material.cAmbient *= m_ka;
//    if(m_ball_velocities.size() == 16 && i != -1 ){
//        o.material.cDiffuse = glm::vec4(glm::abs(m_ball_velocities.at(i)),0.f);
//    }
    o.material.cDiffuse *= m_kd;
    m_phongShader->applyMaterial(o.material);
    m_phongShader->setUniform("m", transform);
    m_simpleDepthShader->setUniform("m", transform);
    if (settings.drawNormals) {
        m_normalsShader->setUniform("m", transform);
        m_normalsArrowShader->setUniform("m", transform);
    }
    if (settings.drawWireframe) {
        m_wireframeShader->setUniform("m", transform);
    }
    applyTextureIfUsed(o);
    applyBumpMappingIfUsed(o);

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
    updateRotation(secondsPassed);
    updateTranslation(secondsPassed);
}

void PoolScene::addVelocity(int ballNum, glm::vec3 vel){
    assert(m_ball_translations.size() == 16);
    m_ball_velocities.at(ballNum) += vel;
}

void PoolScene::collisionDetection(){
    std::vector<glm::vec3> pockets = {glm::vec3(-0.560315,0,1.120315),glm::vec3(0.560315,0,1.120315),
                                     glm::vec3(-0.560315,0,0),glm::vec3(0.560315,0,0),
                                     glm::vec3(-0.560315,0,-1.120315),glm::vec3(0.560315,0,-1.120315)};

    std::set<int> seen;
    for(int i = 0; i < m_ball_velocities.size(); i++){
        if(glm::length(m_ball_velocities[i]) < 0.0001f){
            continue;
        }

        // check intersection with holes
        for(int j = 0; j < pockets.size(); j++){
            if(checkHoleCollision(pockets.at(j),getBallPosition(i))){
                m_ball_done.at(i) = true;
                m_ball_translations.at(i) = glm::vec3(5.f + i, 0.f, 0.f);
                m_ball_velocities.at(i) = glm::vec3(0.f, 0.f, 0.f);
            }
        }

        // check intersection with walls
        checkWallCollision(getBallPosition(i), i);

        // check intersection with other balls
        for(int j = 0; j < m_ball_velocities.size(); j++){
            if(m_ball_done.at(i) || m_ball_done.at(j)) continue;
            int x = i < j? i : j;
            int y = i < j? j: i;
            if(i==j || seen.find(16*x+y) != seen.end()) continue;
            if(checkBallCollision(getBallPosition(x),getBallPosition(y))){
                updateVelocities(x,y);
                seen.insert(16*x+y);
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
//                if(signbit(new_vel.x) != signbit(m_ball_velocities[i].x)){
//                    new_vel.x = 0.f;
//                }
//                if(signbit(new_vel.z) != signbit(m_ball_velocities[i].z)){
//                    new_vel.z = 0.f;
//                }
                m_ball_velocities[i] = new_vel;
            }

            for(int i = 0; i < m_ball_translations.size(); i++){
                m_ball_translations[i] += secondsPassed*m_ball_velocities[i];
            }

    }
}

void PoolScene::updateRotation(float secondsPassed){
    if(m_ball_rotations.size() == 16){
            for(int i = 0; i < m_ball_rotations.size(); i++){

                if(glm::length(m_ball_velocities.at(i)) < 0.0001f){
                    continue;
                }

                glm::vec3 rot_axis = glm::cross(glm::normalize(m_ball_velocities.at(i)),glm::vec3(0.f,1.f,0.f));
                float rot_added = secondsPassed * glm::length(m_ball_velocities[i]) / 0.05715f;
                m_ball_rotations[i] =  glm::rotate(-rot_added,rot_axis) * m_ball_rotations[i];
            }
    }
}

glm::vec3 PoolScene::getBallPosition(int ballnum){
    SceneObject *cue = &m_balls.at(ballnum);
    // TODO: we can just precompute these
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

    float dist = glm::distance(p1,p2);
    glm::vec3 diff = glm::normalize(p1 - p2);
    if(dist < 0.05715f){
        float move = ((0.05715f - dist) / 2.f);
        m_ball_translations[b1] += diff*move;
        m_ball_translations[b2] -= diff*move;
    }
    p1 = getBallPosition(b1);
    p2 = getBallPosition(b2);
    glm::vec3 new_v1 = v1 - glm::dot(v1-v2,p1-p2)*(p1-p2)/glm::pow(glm::length(p1-p2),2.f);
    glm::vec3 new_v2 = v2 - glm::dot(v2-v1,p2-p1)*(p2-p1)/glm::pow(glm::length(p2-p1),2.f);

    m_ball_velocities[b1] = new_v1;
    m_ball_velocities[b2] = new_v2;

    m_ball_dir[b1] = glm::normalize(new_v1);
    m_ball_dir[b2] = glm::normalize(new_v2);
}

bool PoolScene::checkBallCollision(glm::vec3 pos1, glm::vec3 pos2){
    float dist = glm::distance(pos1,pos2);
    float DIAMETER = 0.05715f+0.00005f;
    return dist < DIAMETER;
}

bool PoolScene::checkHoleCollision(glm::vec3 pos1, glm::vec3 pos2){
    float dist = std::sqrt(std::pow(pos1.x - pos2.x, 2) + std::pow(pos1.z - pos2.z, 2));
    //float DIAMETER = 0.028575f+.065f;
    float DIAMETER = .065f;
    return dist < DIAMETER;
}

void PoolScene::checkWallCollision(glm::vec3 pos, int ballnum){
    // lining location - cube radius * z scale - radius of ball
    float z_border = 1.1515f - 0.5f * 0.063f - 0.028575f;
    float x_border = 0.5915f - 0.5f * 0.063f - 0.028575f;

    if(pos.z >= z_border || pos.z <= -z_border){
        m_ball_velocities[ballnum].z *= -1.f;
        m_ball_translations[ballnum].z += pos.z >= z_border? z_border - pos.z: -z_border - pos.z;
        m_ball_dir[ballnum] = glm::normalize(m_ball_velocities[ballnum]);
    }
    else if(pos.x >= x_border || pos.x <= -x_border){
        m_ball_velocities[ballnum].x *= -1.f;
        m_ball_translations[ballnum].x += pos.x >= x_border? x_border - pos.x: -x_border - pos.x;
        m_ball_dir[ballnum] = glm::normalize(m_ball_velocities[ballnum]);
    }
}
