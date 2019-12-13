#include "cubeshape.h"

#include <cmath>
#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"


using namespace CS123::GL;

CubeShape::CubeShape()
    :OpenGLShape()
{
}

CubeShape::~CubeShape() {
}

#define DATAPERVERTEX 8

void CubeShape::makeSides(std::vector<float>* vertices, int p1, int p2) {
    int vertexNum = 2 * p1 * (p1 + 2);
    int dataNum = DATAPERVERTEX*vertexNum;

    firstSide(vertices, p1, 1.f, dataNum); // front

    for (int i = 1; i < 4; i++) {
        rotateSurface(vertices, glm::vec3{0, 1.f, 0}, i * glm::pi<float>()/2, dataNum); // sides
    }
    for (int i = 1; i < 4; i = i + 1) {
        rotateSurface(vertices, glm::vec3{1, 0, 0}, i * glm::pi<float>()/2, dataNum); // sides
    }
}

void CubeShape::firstSide(std::vector<float>* side, int p1, float length, int dataNum) {
    side->resize(dataNum);
    glm::vec2 uv;
    int index = 0;
    for (int i = 0; i < p1; i++) {
        for (int j = 0; j < p1 + 1; j++) {
            side->at(index++) = length/2 - length*j/p1;
            side->at(index++) = length/2 - length*i/p1;
            side->at(index++) = length/2;
            side->at(index++) = 0;
            side->at(index++) = 0;
            side->at(index++) = 1;
            //texcoord
            uv = getUVfromPosition(glm::vec4(side->at(index - 6), side->at(index - 5), side->at(index - 4), 1.f));
            side->at(index++) = uv.x;
            side->at(index++) = uv.y;
            if (j == 0) {
                for (int k = 0; k < DATAPERVERTEX; k++) {
                    side->at(index) = side->at(index - DATAPERVERTEX);
                    index++;
                }
            }
            side->at(index++) = length/2 - length*j/p1;
            side->at(index++) = length/2 - length*(i + 1)/p1;
            side->at(index++) = length/2;
            side->at(index++) = 0;
            side->at(index++) = 0;
            side->at(index++) = 1;
            //texcoord
            uv = getUVfromPosition(glm::vec4(side->at(index - 6), side->at(index - 5), side->at(index - 4), 1.f));
            side->at(index++) = uv.x;
            side->at(index++) = uv.y;
        }
        for (int k = 0; k < DATAPERVERTEX; k++) {
            side->at(index) = side->at(index - DATAPERVERTEX);
            index++;
        }
    }
}

glm::vec2 CubeShape::getUVfromPosition(glm::vec4 point) {
    glm::vec2 uv;

    // XY plane, Z = 0.5
    if (std::abs(point.z - 0.5) <= 0.001) {
        uv = glm::vec2(point.x+0.5, 0.5-point.y);
    }

    // XY plane, Z = -0.5
    else if (std::abs(point.z + 0.5) <= 0.001) {
         uv = glm::vec2(0.5-point.x, 0.5-point.y);
    }

    // XZ plane, Y = 0.5
    else if (std::abs(point.y - 0.5) <= 0.001) {
        uv = glm::vec2(point.x+0.5, point.z+0.5);
    }

    // XZ plane, Y = -0.5
    else if (std::abs(point.y + 0.5) <= 0.001) {
        uv = glm::vec2(point.x+0.5, 0.5-point.z);
    }

    // YZ plane, X = 0.5
    else if (std::abs(point.x - 0.5) <= 0.001) {
        uv = glm::vec2(0.5-point.z, 0.5-point.y);
    }

    // YZ plane, X = -0.5
    else if (std::abs(point.x + 0.5) <= 0.001) {
        uv = glm::vec2(point.z+0.5, 0.5-point.y);
    }

    else {
        throw "Invalid point on cube";
    }

    return uv;
}
