#include "Scene.h"
#include "Camera.h"
#include "CS123ISceneParser.h"
#include "Cone.h"
#include "openglshape.h"
#include <iostream>
#include "cmath"
#include "gl/shaders/Shader.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "glm/gtx/transform.hpp"
#include "settings.h"
#include <math.h>
#include <vector>
#include "utils/TextureUtils.h"

Cone::Cone() :
    CAP(nullptr),
    BODY(nullptr)
{
}

Cone::~Cone()
{
}

/**
 * @brief Cone::createVertices
 * @param p1 is number of triangle pairs per side
 * @param p2 is number of sides
 */
void Cone::createVertices(int p1, int p2) {
    if (p2 < 3) {
        p2 = 3;
    }

    CAP = std::make_unique<Cap>(-1, glm::vec3(0.f, -1.f, 0.f));
    CAP->createVertices(p1, p2);

    std::vector<float> body = createBody(p1, p2);
    BODY = std::make_unique<OpenGLShape>();
    BODY->setVertexData(&body[0], body.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, body.size()/8);
    BODY->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    BODY->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    BODY->setAttribute(ShaderAttrib::TEXCOORD0, 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    BODY->buildVAO();
}

std::vector<float> Cone::createBody(int p1, int p2) {
    std::vector<float> body;
    glm::vec3 tip = glm::vec3(0.f, 0.5f, 0.f);
    std::vector<glm::vec3> points;
    float theta = 2*M_PI / p2;

    for (int i = 0; i < p2; i++) {
        points.push_back(glm::vec3((0.5) * cos(-i * theta), -0.5, (0.5) * sin(-i * theta)));
    }

    for (int i = 0; i < p2; i++) { // one "side" of cone per iter., drawn from top to bottom
        glm::vec3 point1 = points[i];
        glm::vec3 point2;
        if (i == p2-1) {
            point2 = points[0];
        } else {
            point2 = points[i+1];
        }
        float dist = sqrtf(pow(point1[0] - tip[0], 2) + pow(point1[1] - tip[1], 2) + pow(point1[2] - tip[2], 2));
        float offset = dist / p1; // width of triangle pairs
        glm::vec3 slope1 = glm::vec3(tip[0] - point1[0], tip[1] - point1[1], tip[2] - point1[2]);
        glm::vec3 slope2 = glm::vec3(tip[0] - point2[0], tip[1] - point2[1], tip[2] - point2[2]);

        if (i != 0) { // insert first point
            body.insert(body.end(), {point1[0], point1[1], point1[2]});
            // add normal
            float xz_magnitude = sqrtf(pow(point1[0], 2) + pow(point1[2], 2));
            float y_magnitude = xz_magnitude / 2;
            float total_mag = sqrtf(pow(point1[0], 2) + pow(y_magnitude, 2) + pow(point1[2], 2));
            body.insert(body.end(), {point1[0] / total_mag, y_magnitude / total_mag, point1[2] / total_mag});
            // add uv
            glm::vec2 uv = TextureUtils::cylinderToUV(glm::vec4(point1.xyz(), 1));
            body.insert(body.end(), {uv[0], uv[1]});
        }
        for (int j = 0; j < p1; j++) { // does not include triangle at tip of cone
            float offset_norm = offset / dist;
            glm::vec3 point = glm::vec3(point1[0] + (offset_norm*j) * slope1[0], point1[1] + (offset_norm*j) * slope1[1], point1[2] + (offset_norm*j) * slope1[2]);
            body.insert(body.end(), {point[0], point[1], point[2]});
            // add normal
            float xz_magnitude = sqrtf(pow(point1[0], 2) + pow(point1[2], 2));
            float y_magnitude = xz_magnitude / 2;
            float total_mag = sqrtf(pow(point1[0], 2) + pow(y_magnitude, 2) + pow(point1[2], 2));
            body.insert(body.end(), {point1[0] / total_mag, y_magnitude / total_mag, point1[2] / total_mag});
            // add uv
            glm::vec2 uv = TextureUtils::cylinderToUV(glm::vec4(point.xyz(), 1));
            body.insert(body.end(), {uv[0], uv[1]});

            point = glm::vec3(point2[0] + (offset_norm*j) * slope2[0], point2[1] + (offset_norm*j) * slope2[1], point2[2] + (offset_norm*j) * slope2[2]);
            body.insert(body.end(), {point2[0] + (offset_norm*j) * slope2[0], point2[1] + (offset_norm*j) * slope2[1], point2[2] + (offset_norm*j) * slope2[2]});
            // add normal
            xz_magnitude = sqrtf(pow(point2[0], 2) + pow(point2[2], 2));
            y_magnitude = xz_magnitude / 2;
            total_mag = sqrtf(pow(point2[0], 2) + pow(y_magnitude, 2) + pow(point2[2], 2));
            body.insert(body.end(), {point2[0] / total_mag, y_magnitude / total_mag, point2[2] / total_mag});
            // add uv
            uv = TextureUtils::cylinderToUV(glm::vec4(point.xyz(), 1));
            body.insert(body.end(), {uv[0], uv[1]});
        }
        glm::vec3 point = glm::vec3(0.f, 0.5, 0.f);
        body.insert(body.end(), {point[0], point[1], point[2]}); // tip
        glm::vec3 cp = crossProduct(slope1, slope2);
        glm::vec3 norm_cp = normalize(cp);
        body.insert(body.end(), {norm_cp[0], norm_cp[1], norm_cp[2]}); // normal
        glm::vec2 uv = TextureUtils::cylinderToUV(glm::vec4(point.xyz(), 1));
        body.insert(body.end(), {uv[0], uv[1]});

        if (i != p2 - 1) { // add last point
            glm::vec3 point = glm::vec3(0.f, 0.5, 0.f);
            body.insert(body.end(), {point[0], point[1], point[2]});
            body.insert(body.end(), {norm_cp[0], norm_cp[1], norm_cp[2]}); // normal
            uv = TextureUtils::cylinderToUV(glm::vec4(point.xyz(), 1)); // uv
            body.insert(body.end(), {uv[0], uv[1]});

            point = glm::vec3(0.f, 0.5, 0.f);
            body.insert(body.end(), {point[0], point[1], point[2]});
            body.insert(body.end(), {norm_cp[0], norm_cp[1], norm_cp[2]}); // normal
            uv = TextureUtils::cylinderToUV(glm::vec4(point.xyz(), 1)); // uv
            body.insert(body.end(), {uv[0], uv[1]});
        }
    }
    return body;
}

glm::vec3 Cone::crossProduct(glm::vec3 x, glm::vec3 y) {
   return glm::vec3(x[1]*y[2] - x[2]*y[1], x[2]*y[0] - x[0]*y[2], x[0]*y[1] - x[1]*y[0]);
}

glm::vec3 Cone::normalize(glm::vec3 x) {
   float mag = sqrtf(pow(x[0], 2) + pow(x[1], 2) + pow(x[2], 2));
   return glm::vec3(x[0]/mag, x[1]/mag, x[2]/mag);
}

void Cone::drawShape() {
    BODY->draw();
    CAP->drawShape();
}
