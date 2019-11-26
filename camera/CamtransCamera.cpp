/**
 * @file   CamtransCamera.cpp
 *
 * This is the perspective camera class you will need to fill in for the Camtrans lab.  See the
 * lab handout for more details.
 */

#include "CamtransCamera.h"
#include <Settings.h>
#include "glm/matrix.hpp"

CamtransCamera::CamtransCamera() :
    m_aspectRatio(1.f),
    m_near(1.f),
    m_far(30.f),
    m_thetaH(glm::radians(60.f)),
    m_eye(glm::vec4(2, 2, 2, 1)),
    m_up(glm::vec4(0, 1, 0, 0)),
    m_w(glm::vec4(2, 2, 2, 0))
{
    // @TODO: [CAMTRANS] Fill this in...
}

void CamtransCamera::setAspectRatio(float a)
{
    m_aspectRatio = a;
    updateProjectionMatrix();
}

glm::mat4x4 CamtransCamera::getProjectionMatrix() const {
    return  m_perspectiveTransformation *  m_scaleMatrix;
}

glm::mat4x4 CamtransCamera::getViewMatrix() const {
    return  m_rotationMatrix * m_translationMatrix;
}

glm::mat4x4 CamtransCamera::getScaleMatrix() const {
    return m_scaleMatrix;
}

glm::mat4x4 CamtransCamera::getPerspectiveMatrix() const {
    return m_perspectiveTransformation;
}

glm::vec4 CamtransCamera::getPosition() const {
    return m_eye;
}

glm::vec4 CamtransCamera::getLook() const {
    return -m_w;
}

glm::vec4 CamtransCamera::getUp() const {
    return m_up;
}

float CamtransCamera::getAspectRatio() const {
    return m_aspectRatio;
}

float CamtransCamera::getHeightAngle() const {
    return m_thetaH;
}

void CamtransCamera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up) {
    m_eye = eye;
    m_up = up;
    m_w = glm::normalize(-look);
    m_v = glm::normalize(up - glm::dot(up.xyz(), m_w.xyz()) * m_w);
    m_u = glm::normalize(glm::vec4(glm::cross(m_v.xyz(), m_w.xyz()), 0));

    updateViewMatrix();
    updateProjectionMatrix();
}

void CamtransCamera::setHeightAngle(float h) {
    m_thetaH = glm::radians(h);
    updateProjectionMatrix();
}

void CamtransCamera::translate(const glm::vec4 &v) {
    m_eye = m_eye + v;
    updateViewMatrix();
}

void CamtransCamera::rotateU(float degrees) {
    float rad = glm::radians(degrees);
    glm::vec4 v0 = m_v;
    glm::vec4 w0 = m_w;
    m_v = w0 * glm::sin(rad) + v0 * glm::cos(rad);
    m_w = w0 * glm::cos(rad) - v0 * glm::sin(rad);
    updateViewMatrix();

}

void CamtransCamera::rotateV(float degrees) {
    float rad = glm::radians(degrees);
    glm::vec4 u0 = m_u;
    glm::vec4 w0 = m_w;
    m_u = u0 * glm::cos(rad) - w0 * glm::sin(rad);
    m_w = u0 * glm::sin(rad) + w0 * glm::cos(rad);
    updateViewMatrix();

}

void CamtransCamera::rotateW(float degrees) {
    float rad = glm::radians(degrees);
    rad = -1 * rad;
    glm::vec4 u0 = m_u;
    glm::vec4 v0 = m_v;
    m_u = u0 * glm::cos(rad) - v0 * glm::sin(rad);
    m_v = u0 * glm::sin(rad) + v0 * glm::cos(rad);
    updateViewMatrix();
}

void CamtransCamera::setClip(float nearPlane, float farPlane) {
    m_near = nearPlane;
    m_far = farPlane;
    updateProjectionMatrix();
}

glm::vec4 CamtransCamera::getU() const {
    return m_u;
}

glm::vec4 CamtransCamera::getV() const {
    return m_v;
}

glm::vec4 CamtransCamera::getW() const {
    return m_w;
}

void CamtransCamera::updateProjectionMatrix(){
    updateScaleMatrix();
    updatePerspectiveMatrix();
}

void CamtransCamera::updatePerspectiveMatrix(){
    float c = -m_near / m_far;
    m_perspectiveTransformation = glm::mat4(1.f, 0.f, 0.f, 0.f,
                                            0.f, 1.f, 0.f, 0.f,
                                            0.f, 0.f,  -1.f/(c + 1.f), -1.f,
                                            0.f, 0.f, c/(c+1.f), 0.f);
}

void CamtransCamera::updateScaleMatrix(){
    m_scaleMatrix = glm::mat4(1.f/(m_aspectRatio * glm::tan(m_thetaH/2) * m_far), 0.f, 0.f, 0.f,
                              0.f, 1.f/(glm::tan(m_thetaH/2) * m_far), 0.f, 0.f,
                              0.f, 0.f, 1.f/m_far, 0.f,
                              0.f, 0.f, 0.f, 1.f);
}


void CamtransCamera::updateViewMatrix(){
    updateTranslationMatrix();
    updateRotationMatrix();
}

void CamtransCamera::updateRotationMatrix(){
    m_rotationMatrix = glm::mat4(m_u.x, m_v.x, m_w.x, 0.f,
                                 m_u.y, m_v.y, m_w.y, 0.f,
                                 m_u.z, m_v.z, m_w.z, 0.f,
                                 0.f, 0.f, 0.f, 1.f);
}

void CamtransCamera::updateTranslationMatrix(){
    m_translationMatrix = glm::mat4(1.f, 0.f, 0.f, 0.f,
                                    0.f, 1.f, 0.f, 0.f,
                                    0.f, 0.f, 1.f, 0.f,
                                    -1.f * m_eye.x, -1.f * m_eye.y, -1.f * m_eye.z, 1.f);
}
