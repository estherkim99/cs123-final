#include "sphereshape.h"

using namespace CS123::GL;

SphereShape::SphereShape()
{
}

SphereShape::~SphereShape() {
}


void SphereShape::makeSides(std::vector<float>* vertices, int p1, int p2) {
    if (p1 == 1) {
        p1 = 2;
    }
    if (p2 < 4) {
        p2 = 3;
    }

    int dataPerVertex = 6;
    int vertexNum = 4 * p1 + 2 * p1;
    int dataNum = dataPerVertex*vertexNum;
    float pi = glm::pi<float>();

    float length = pi / p2;
    firstSide(vertices, p1, p2, length, dataNum);
    for (int i = 0; i < p2 - 1; i++) {
        rotateSurface(vertices, glm::vec3{0, 1.f, 0}, 2.f * pi * (i+1) / p2, dataNum);
    }
}

void SphereShape::firstSide(std::vector<float>* side, int p1, int p2, float length, int dataNum) {
    float theta = glm::pi<float>() / p1;
    int index = 0;
    side->resize(dataNum);
    for (int i = 0; i < p1; i++) {
        for (int j = 0; j < 2; j++) {
            side->at(index++) = length * sinf(theta * i) * (0.5f - j);
            side->at(index++) = 0.5 * cosf(theta * i);
            side->at(index++) = length * sinf(theta * i) / (tanf(glm::pi<float>()/p2)) /2;

            glm::vec3 norm = glm::vec3(side->at(index-3), side->at(index-2), side->at(index-1));
            norm = glm::normalize(norm);
            side->at(index++) = norm.x;
            side->at(index++) = norm.y;
            side->at(index++) = norm.z;
            if (j == 0) {
                for (int k = 0; k < 6; k++) {
                    side->at(index) = side->at(index - 6);
                    index++;
                }
            }
            side->at(index++) = length * sinf(theta * (i + 1)) * (0.5f - j);
            side->at(index++) = 0.5 * cosf(theta * (i + 1));
            side->at(index++) = length * sinf(theta* (i + 1)) / (tanf(glm::pi<float>()/p2)) / 2;
            norm = glm::vec3(side->at(index-3), side->at(index-2), side->at(index-1));
            norm = glm::normalize(norm);
            side->at(index++) = norm.x;
            side->at(index++) = norm.y;
            side->at(index++) = norm.z;
        }
        for (int k = 0; k < 6; k++) {
            side->at(index) = side->at(index - 6);
            index++;
        }
    }
}