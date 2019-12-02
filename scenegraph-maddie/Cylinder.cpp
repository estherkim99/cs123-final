#include "Scene.h"
#include "Camera.h"
#include "CS123ISceneParser.h"
#include "Cube.h"
#include "openglshape.h"
#include <iostream>
#include "cmath"
#include "gl/shaders/Shader.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "glm/gtx/transform.hpp"
#include "settings.h"
#include <math.h>
#include <vector>
#include "openglshape.h"
#include "Cylinder.h"
#include "Cap.h"

Cylinder::Cylinder() :
    TOP_CAP(nullptr),
    BOT_CAP(nullptr),
    BARREL(nullptr)
{
}

Cylinder::~Cylinder()
{
}

/**
 * @brief Cylinder::createVertices
 * @param p1 is number of sides
 * @param p2 is number of triangle pairs per side
 */
void Cylinder::createVertices(int p1, int p2) {
    if (p2 < 3) {
        p2 = 3;
    }

    TOP_CAP = std::make_unique<Cap>(1, glm::vec3(0.f, 1.f, 0.f));
    TOP_CAP->createVertices(p1, p2);

    BOT_CAP = std::make_unique<Cap>(-1, glm::vec3(0.f, -1.f, 0.f));
    BOT_CAP->createVertices(p1, p2);

    std::vector<float> barrel = createBarrel(p1, p2, 1);
    BARREL = std::make_unique<OpenGLShape>();
    BARREL->setVertexData(&barrel[0], barrel.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, barrel.size()/6);
    BARREL->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    BARREL->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    BARREL->buildVAO();
}

std::vector<float> Cylinder::createBarrel(int p1, int p2, int multiplier) {
    std::vector<float> barrel;
    std::vector<glm::vec3> points;
    float theta = 2*M_PI / p2;
    for (int i = 0; i < p2; i++) {
        points.push_back(glm::vec3((0.5) * cos(i * theta), 0.5, (0.5) * sin(multiplier * i * theta)));
    }

    for (int j = 0; j < p2; j++) { // one "side" of barrel per iter., drawn from top to bottom
        for (int k = 0; k < p1 + 1; k++) {
            float offset = 1.f / p1;
            barrel.insert(barrel.end(), {points[j][0], points[j][1] - k * offset, points[j][2]});
            barrel.insert(barrel.end(), {2 * points[j][0], 0, 2 * points[j][2]}); // normal

            if (j != p2 - 1) { // if not on last side, go to next point
                barrel.insert(barrel.end(), {points[j+1][0], points[j+1][1] - k * offset, points[j+1][2]});
                barrel.insert(barrel.end(), {2 * points[j+1][0], 0, 2 * points[j+1][2]}); // normal
            } else { // else, go to first point
                barrel.insert(barrel.end(), {points[0][0], points[0][1] - k * offset, points[0][2]});
                barrel.insert(barrel.end(), {2 * points[0][0], 0, 2 * points[0][2]}); // normal
            }
        }
        // TODO (add degen's after each side)
    }
    return barrel;
}

void Cylinder::drawShape() {
    TOP_CAP->drawShape();
    BOT_CAP->drawShape();
    BARREL->draw();
}
