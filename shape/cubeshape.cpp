#include "cubeshape.h"

#include <cmath>
#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"
#include <iostream>

using namespace CS123::GL;

CubeShape::CubeShape()
    :OpenGLShape()
{
}

CubeShape::~CubeShape() {
}

void CubeShape::makeSides(std::vector<float>* vertices, int p1, int p2) {
    int dataPerVertex = 8;
    int vertexNum = 2 * p1 * (p1 + 2);
    int dataNum = dataPerVertex*vertexNum;

    firstSide(vertices, p1, 1.f, dataNum);

    for (int i = 1; i < 4; i++) {
        rotateSurface(vertices, glm::vec3{0, 1.f, 0}, i * glm::pi<float>()/2, dataNum); // sides
    }
    for (int i = 1; i < 4; i = i + 1) {
        rotateSurface(vertices, glm::vec3{1, 0, 0}, i * glm::pi<float>()/2, dataNum); // sides
    }
}

void CubeShape::firstSide(std::vector<float>* side, int p1, float length, int dataNum) {
    side->resize(dataNum);
    int index = 0;
    for (int i = 0; i < p1; i++) {
        for (int j = 0; j < p1 + 1; j++) {
            side->at(index++) = length/2 - length*j/p1;
            side->at(index++) = length/2 - length*i/p1;
            side->at(index++) = length/2;
            side->at(index++) = 0;
            side->at(index++) = 0;
            side->at(index++) = 1;
            // add uv coords for texture mapping
            glm::vec2 uv = getUV(glm::vec4(length/2 - length*j/p1, length/2 - length*i/p1, length/2, 1),
                                 glm::vec4(0, 0, 1, 0));
//            side->at(index++) = uv.x;
//            side->at(index++) = uv.y;

            if (j == 0) {
                for (int k = 0; k < 6; k++) {
                    side->at(index) = side->at(index - 6);
                    index++;
                }
                // degenerate, so uv's don't matter
//                side->at(index++) = 0;
//                side->at(index++) = 0;
            }
            side->at(index++) = length/2 - length*j/p1;
            side->at(index++) = length/2 - length*(i + 1)/p1;
            side->at(index++) = length/2;
            side->at(index++) = 0;
            side->at(index++) = 0;
            side->at(index++) = 1;
            uv = getUV(glm::vec4(length/2 - length*j/p1, length/2 - length*(i + 1)/p1, length/2, 1),
                                 glm::vec4(0, 0, 1, 0));
//            side->at(index++) = uv.x;
//            side->at(index++) = uv.y;
        }
        for (int k = 0; k < 6; k++) {
            side->at(index) = side->at(index - 6);
            index++;
        }
        // degenerate, so uv's don't matter
//        side->at(index++) = 0;
//        side->at(index++) = 0;
    }
}

glm::vec2 CubeShape::getUV(glm::vec4 p, glm::vec4 n)
{
    glm::vec2 ret = glm::vec2(-1.f);

    if (abs(n.x - 1.f) < 0.000001f) {
        ret = glm::vec2(0.5f - p.z, 0.5f - p.y);
    }
    if (abs(n.x + 1.f) < 0.000001f) {
        ret = glm::vec2(0.5f + p.z, 0.5f - p.y);
    }
    if (abs(n.y - 1.f) < 0.000001f) {
        ret = glm::vec2(0.5f + p.x, 0.5f + p.z);
    }
    if (abs(n.y + 1.f) < 0.000001f) {
        ret = glm::vec2(0.5f + p.x, 0.5f - p.z);
    }
    if (abs(n.z - 1.f) < 0.000001f) {
        ret = glm::vec2(0.5f + p.x, 0.5f - p.y);
    }
    if (abs(n.z + 1.f) < 0.000001f) {
        ret = glm::vec2(0.5f - p.x, 0.5f - p.y);
    }
    return ret;
}
