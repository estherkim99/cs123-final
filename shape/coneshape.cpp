#include "coneshape.h"

ConeShape::ConeShape()
{
}
ConeShape::~ConeShape() {

}

void ConeShape::makeSides(std::vector<float>* vertices, int p1, int p2) {
    int dataPerVertex = 6;
    int vertexNum = 4 * p1 + 8;
    int dataNum = dataPerVertex*vertexNum;
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

void ConeShape::firstSide(std::vector<float>* side, int p1, int p2, float length, int dataNum) {
    side->resize(dataNum);
    //bottom
    int index = 0;
    for (int i = 0; i < p1 + 1; i++) {
        side->at(index++) = length/2/p1 * i;
        side->at(index++) = -0.5;
        side->at(index++) = length / (2 * tanf(glm::pi<float>()/p2)) / p1 * i;
        side->at(index++) = 0;
        side->at(index++) = -1;
        side->at(index++) = 0;
        if (i == 0) {
            for (int k = 0; k < 6; k++) {
                side->at(index) = side->at(index - 6);
                index++;
            }
        }
        side->at(index++) = -length/2/p1 * i;
        for (int k = 0; k < 5; k++) {
            side->at(index) = side->at(index - 6);
            index++;
        }
    }
    for (int k = 0; k < 6; k++) {
        side->at(index) = side->at(index - 6);
        index++;
    }

    side->at(index++) = 0;
    side->at(index++) = 0.5f;
    side->at(index++) = 0;
    side->at(index++) = 0;
    side->at(index++) = 1/sqrtf(5);
    side->at(index++) = 2/sqrtf(5);

    for (int k = 0; k < 6; k++) {
        side->at(index) = side->at(index - 6);
        index++;
    }
    for (int i = 1; i < p1 + 1; i++) {
        for (int j = 0; j < 2; j++) {
            side->at(index++) = length * (0.5f - j) * (1.f * i/p1);
            side->at(index++) = 0.5 - 1.f *i/p1;
            side->at(index++) = length / (2 * tanf(glm::pi<float>()/p2)) * (1.f * i/p1);
            glm::vec3 norm = glm::vec3(side->at(index-3), 0, side->at(index-1));
            norm = glm::normalize(norm);
            side->at(index++) = norm.x * sqrtf(2/sqrtf(5));;
            side->at(index++) = 1/sqrtf(5);
            side->at(index++) = norm.z * sqrtf(2/sqrtf(5));;
        }
    }
    for (int k = 0; k < 12; k++) {
        side->at(index) = side->at(index - 6);
        index++;
    }
}
