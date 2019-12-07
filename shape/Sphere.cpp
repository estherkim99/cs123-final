#include "Scene.h"
#include "Camera.h"
#include "CS123ISceneParser.h"
#include "Sphere.h"
#include "openglshape.h"
#include <iostream>
#include "cmath"
#include "gl/shaders/Shader.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "glm/gtx/transform.hpp"
#include <math.h>
#include <vector>
#include "utils/TextureUtils.h"

Sphere::Sphere() :
    BODY(nullptr)
{
}

Sphere::~Sphere()
{
}

/**
 * @brief Sphere::createVertices
 * @param p1 is phi
 * @param p2 is theta
 */
void Sphere::createVertices(int p1, int p2) {
    if (p2 < 3) {
        p2 = 3;
    }
    if (p1 < 2) {
        p1 = 2;
    }

    std::vector<float> body = createBody(p1, p2);
    BODY = std::make_unique<OpenGLShape>();
    BODY->setVertexData(&body[0], body.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, body.size()/8); // why was this 3 before?
    BODY->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    BODY->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    BODY->setAttribute(ShaderAttrib::TEXCOORD0, 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false); // uv coords
    BODY->buildVAO();
}

/**
 * @brief Sphere::createVertices
 * @param p1 is phi
 * @param p2 is theta
 */
std::vector<float> Sphere::createBody(int p1, int p2) {
    std::vector<float> body;
    glm::vec3 top = glm::vec3(0.f, 0.5f, 0.f);
    glm::vec3 bot = glm::vec3(0.f, -0.5f, 0.f);
    float phi = M_PI / p1;
    float theta = 2*M_PI / p2;

    for (int i = 0; i < p2; i++) { // one "side" of the sphere per iter., drawn from top to bottom
        body.insert(body.end(), {top[0], top[1],top[2]});
        body.insert(body.end(), {2*top[0], 2*top[1], 2*top[2]}); // normal
        glm::vec2 uv = TextureUtils::sphereToUV(glm::vec4(top, 1));
        body.insert(body.end(), {uv[0], uv[1]}); // uv

        body.insert(body.end(), {top[0], top[1],top[2]});
        body.insert(body.end(), {2*top[0], 2*top[1], 2*top[2]}); // normal
        body.insert(body.end(), {uv[0], uv[1]}); // uv

        for (int j = 0; j < p1-1; j++) {
            float curr_phi = phi * (j+1);
            float curr_theta = theta * i;
            glm::vec3 point = glm::vec3(static_cast<float>(0.5f * sin(curr_phi) * cos(curr_theta)),
                                        static_cast<float>(0.5f * cos(curr_phi)),
                                        static_cast<float>(0.5f * sin(curr_phi) * sin(curr_theta)));
            body.insert(body.end(), {point[0], point[1], point[2]});
            body.insert(body.end(), {
                            static_cast<float>(2 * 0.5f * sin(curr_phi) * cos(curr_theta)),
                            static_cast<float>(2 * 0.5f * cos(curr_phi)),
                            static_cast<float>(2 * 0.5f * sin(curr_phi) * sin(curr_theta))}); // normal
            glm::vec2 uv = TextureUtils::sphereToUV(glm::vec4(point, 1));
            body.insert(body.end(), {uv[0], uv[1]}); // uv

            curr_theta = theta * (i+1);
            point = glm::vec3(static_cast<float>(0.5f * sin(curr_phi) * cos(curr_theta)),
                              static_cast<float>(0.5f * cos(curr_phi)),
                              static_cast<float>(0.5f * sin(curr_phi) * sin(curr_theta)));
            body.insert(body.end(), {point[0], point[1], point[2]});
            body.insert(body.end(), {
                            static_cast<float>(2 * 0.5f * sin(curr_phi) * cos(curr_theta)),
                            static_cast<float>(2 * 0.5f * cos(curr_phi)),
                            static_cast<float>(2 * 0.5f * sin(curr_phi) * sin(curr_theta))}); // normal
            uv = TextureUtils::sphereToUV(glm::vec4(point, 1));
            body.insert(body.end(), {uv[0], uv[1]}); // uv
        }

        body.insert(body.end(), {bot[0], bot[1],bot[2]});
        body.insert(body.end(), {2*bot[0], 2*bot[1], 2*bot[2]}); // normal
        uv = TextureUtils::sphereToUV(glm::vec4(bot, 1));
        body.insert(body.end(), {uv[0], uv[1]}); // uv

        body.insert(body.end(), {bot[0], bot[1],bot[2]});
        body.insert(body.end(), {2*bot[0], 2*bot[1], 2*bot[2]}); // normal
        uv = TextureUtils::sphereToUV(glm::vec4(bot, 1));
        body.insert(body.end(), {uv[0], uv[1]});
    }
    return body;
}

void Sphere::drawShape() {
    BODY->draw();
}
