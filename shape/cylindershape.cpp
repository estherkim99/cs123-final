#include "cylindershape.h"

#include <iostream>

using namespace CS123::GL;

CylinderShape::CylinderShape() : OpenGLShape ()
{

}

CylinderShape::~CylinderShape(){
}

void CylinderShape::makeSides(std::vector<float>* vertices, int p1, int p2) {
    int vertexNum = 10 * p1 + 8;
    int dataNum = DATAPERVERTEX*vertexNum;
    float pi = glm::pi<float>();

    if (p2 < 4) {
        p2 = 3;
    }

    float length = sinf(pi / p2);
    firstSide(vertices, p1, p2, length, dataNum);
    for (int i = 0; i < p2 - 1; i++) {
        rotateSurface(vertices, glm::vec3{0, 1.f, 0}, 2.f * pi * (i+1) / p2, dataNum); // bottom
    }
}

void CylinderShape::firstSide(std::vector<float>* side, int p1, int p2, float length, int dataNum) {
    side->resize(dataNum);
    //top
    int index = 0;
    for (int i = 0; i < p1 + 1; i++) {
        side->at(index++) = -length/2/p1 * i;
        side->at(index++) = 0.5;
        side->at(index++) = length / (2 * tanf(glm::pi<float>()/p2)) / p1 * i;
        side->at(index++) = 0;
        side->at(index++) = 1;
        side->at(index++) = 0;
        // texcoord, tangent, binormal
        for (int j = 0; j < 8; j++) {
            side->at(index++) = 0;
        }
        if (i == 0) {
            for (int k = 0; k < DATAPERVERTEX; k++) {
                side->at(index) = side->at(index - DATAPERVERTEX);
                index++;
            }
        }
        side->at(index++) = length/2/p1 * i;
        for (int k = 0; k < DATAPERVERTEX - 9; k++) {
            side->at(index) = side->at(index - DATAPERVERTEX);
            index++;
        }
        // texcoord, tangent, binormal
        for (int j = 0; j < 8; j++) {
            side->at(index++) = 0;
        }
    }
    for (int k = 0; k < DATAPERVERTEX; k++) {
        side->at(index) = side->at(index - DATAPERVERTEX);
        index++;
    }

    // barrel
    for (int i = 0; i < p1; i++) {
        for (int j = 0; j < 2; j++) {
            side->at(index++) = length * (0.5f - j);
            side->at(index++) = 0.5 - 1.f *i/p1;
            side->at(index++) = length / (2 * tanf(glm::pi<float>()/p2));
            glm::vec3 norm = glm::vec3(side->at(index-3), 0, side->at(index-1));
            side->at(index++) = norm.x;
            side->at(index++) = 0;
            side->at(index++) = norm.z;
            // texcoord, tangent, binormal
            for (int j = 0; j < 8; j++) {
                side->at(index++) = 0;
            }
            if (j == 0) {
                for (int k = 0; k < DATAPERVERTEX; k++) {
                    side->at(index) = side->at(index - DATAPERVERTEX);
                    index++;
                }
            }
            side->at(index++) = length * (0.5f - j);
            side->at(index++) = 0.5 - 1.f * (i + 1)/p1;
            for (int k = 0; k < DATAPERVERTEX - 10; k++) {
                side->at(index) = side->at(index - DATAPERVERTEX);
                index++;
            }
            // texcoord, tangent, binormal
            for (int j = 0; j < 8; j++) {
                side->at(index++) = 0;
            }
        }
        for (int k = 0; k < DATAPERVERTEX; k++) {
            side->at(index) = side->at(index - DATAPERVERTEX);
            index++;
        }
    }

    // bottom
    for (int k = 0; k < DATAPERVERTEX * (2 * p1 + 4); k = k + 8) {
        side->at(index++) = -side->at(k++);
        side->at(index++) = -side->at(k++);
        side->at(index++) = side->at(k++);
        side->at(index++) = -side->at(k++);
        side->at(index++) = -side->at(k++);
        side->at(index++) = side->at(k++);
        // texcoord, tangent, binormal
        for (int j = 0; j < 8; j++) {
            side->at(index++) = 0;
        }
    }
}

glm::vec2 CylinderShape::getUVfromPosition(glm::vec4 point) {
    float u;
    float theta = atan2(point.z, point.x);

    if (theta < 0) {
        u = -theta / (2*M_PI);
    } else {
        u = 1.f - (theta / (2*M_PI));
    }
    return glm::vec2(u, 0.5-point.y);
}
