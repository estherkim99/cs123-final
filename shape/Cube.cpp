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
#include "utils/TextureUtils.h"

Cube::Cube() :
    POS_XY(nullptr),
    NEG_XY(nullptr),
    POS_ZY(nullptr),
    NEG_ZY(nullptr),
    POS_XZ(nullptr),
    NEG_XZ(nullptr)
{
}

Cube::~Cube()
{
}

void Cube::createVertices(int p1, int p2) {
    std::vector<float> pos_xy = createFace(p1, glm::vec3 {0, 0, 1}, glm::vec3 {-0.5, 0.5, 0.5}, glm::vec3 {0.5, 0.5, 0.5}, glm::vec3 {-0.5, -0.5, 0.5}, glm::vec3 {0.5, -0.5, 0.5});
    std::vector<float> neg_xy = createFace(p1, glm::vec3 {0, 0, -1}, glm::vec3 {0.5, 0.5, -0.5}, glm::vec3 {-0.5, 0.5, -0.5}, glm::vec3 {0.5, -0.5, -0.5}, glm::vec3 {-0.5, -0.5, -0.5});
    std::vector<float> pos_zy = createFace(p1, glm::vec3 {1, 0, 0}, glm::vec3 {0.5, 0.5, 0.5}, glm::vec3 {0.5, 0.5, -0.5}, glm::vec3 {0.5, -0.5, 0.5}, glm::vec3 {0.5, -0.5, -0.5});
    std::vector<float> neg_zy = createFace(p1, glm::vec3 {-1, 0, 0}, glm::vec3 {-0.5, 0.5, -0.5}, glm::vec3 {-0.5, 0.5, 0.5}, glm::vec3 {-0.5, -0.5, -0.5}, glm::vec3 {-0.5, -0.5, 0.5});
    std::vector<float> pos_xz = createFace(p1, glm::vec3 {0, 1, 0}, glm::vec3 {0.5, 0.5, -0.5}, glm::vec3 {0.5, 0.5, 0.5}, glm::vec3 {-0.5, 0.5, -0.5}, glm::vec3 {-0.5, 0.5, 0.5});
    std::vector<float> neg_xz = createFace(p1, glm::vec3 {0, -1, 0}, glm::vec3 {0.5, -0.5, 0.5}, glm::vec3 {0.5, -0.5, -0.5}, glm::vec3 {-0.5, -0.5, 0.5}, glm::vec3 {-0.5, -0.5, -0.5});

    POS_XY = std::make_unique<OpenGLShape>();
    POS_XY->setVertexData(&pos_xy[0], pos_xy.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, pos_xy.size()/8);
    POS_XY->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    POS_XY->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    POS_XY->setAttribute(ShaderAttrib::TEXCOORD0, 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    POS_XY->buildVAO();

    NEG_XY = std::make_unique<OpenGLShape>();
    NEG_XY->setVertexData(&neg_xy[0], neg_xy.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, neg_xy.size()/8);
    NEG_XY->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    NEG_XY->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    NEG_XY->setAttribute(ShaderAttrib::TEXCOORD0, 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    NEG_XY->buildVAO();

    POS_ZY = std::make_unique<OpenGLShape>();
    POS_ZY->setVertexData(&pos_zy[0], pos_zy.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, pos_zy.size()/8);
    POS_ZY->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    POS_ZY->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    POS_ZY->setAttribute(ShaderAttrib::TEXCOORD0, 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    POS_ZY->buildVAO();

    NEG_ZY = std::make_unique<OpenGLShape>();
    NEG_ZY->setVertexData(&neg_zy[0], neg_zy.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, neg_zy.size()/8);
    NEG_ZY->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    NEG_ZY->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    NEG_ZY->setAttribute(ShaderAttrib::TEXCOORD0, 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    NEG_ZY->buildVAO();

    POS_XZ = std::make_unique<OpenGLShape>();
    POS_XZ->setVertexData(&pos_xz[0], pos_xz.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, pos_xz.size()/8);
    POS_XZ->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    POS_XZ->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    POS_XZ->setAttribute(ShaderAttrib::TEXCOORD0, 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    POS_XZ->buildVAO();

    NEG_XZ = std::make_unique<OpenGLShape>();
    NEG_XZ->setVertexData(&neg_xz[0], neg_xz.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, neg_xz.size()/8);
    NEG_XZ->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    NEG_XZ->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    NEG_XZ->setAttribute(ShaderAttrib::TEXCOORD0, 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    NEG_XZ->buildVAO();
}

std::vector<float> Cube::createFace(int p1, glm::vec3 normal, glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 botLeft, glm::vec3 botRight) {
    std::vector<float> vector;
    glm::vec3 width = topRight - topLeft;
    glm::vec3 w_offset = glm::vec3(width[0]/p1, width[1]/p1, width[2]/p1);
    glm::vec3 height = botLeft - topLeft;
    glm::vec3 h_offset = glm::vec3(height[0]/p1, height[1]/p1, height[2]/p1);

    std::vector<float> second;
    std::vector<float> last;
    for (int i = 0; i < p1; i++) {
        for (int j = 0; j < p1+1; j++) {
            glm::vec3 point = glm::vec3(topLeft[0] + i * h_offset[0] + j * w_offset[0],
                                        topLeft[1] + i * h_offset[1] + j * w_offset[1],
                                        topLeft[2] + i * h_offset[2] + j * w_offset[2]);
            glm::vec2 uv = TextureUtils::cubeToUV(glm::vec4(point.xyz(), 1));
            vector.insert(vector.end(), {point[0], point[1], point[2]});
            vector.insert(vector.end(), {normal[0], normal[1], normal[2]});
            vector.insert(vector.end(), {uv[0], uv[1]}); // uv

            point = glm::vec3(topLeft[0] + (i+1) * h_offset[0] + j * w_offset[0],
                              topLeft[1] + (i+1) * h_offset[1] + j * w_offset[1],
                              topLeft[2] + (i+1) * h_offset[2] + j * w_offset[2]);
            uv = TextureUtils::cubeToUV(glm::vec4(point.xyz(), 1));
            vector.insert(vector.end(), {point[0], point[1], point[2]});
            vector.insert(vector.end(), {normal[0], normal[1], normal[2]});
            vector.insert(vector.end(), {uv[0], uv[1]}); // uv

            // keep track of degenerates
            if (j == 0) {
                second = std::vector<float> {
                    topLeft[0] + (i+1) * h_offset[0] + j * w_offset[0],
                    topLeft[1] + (i+1) * h_offset[1] + j * w_offset[1],
                    topLeft[2] + (i+1) * h_offset[2] + j * w_offset[2]
                };
            } else if (j == p1) {
                last = std::vector<float> {
                    topLeft[0] + (i+1) * h_offset[0] + j * w_offset[0],
                    topLeft[1] + (i+1) * h_offset[1] + j * w_offset[1],
                    topLeft[2] + (i+1) * h_offset[2] + j * w_offset[2]
                };
            }
        }

        // if not at last row, add degenerates
        if (i != p1-1) {
            vector.insert(vector.end(), {last[0], last[1], last[2]});
            vector.insert(vector.end(), {normal[0], normal[1], normal[2]});
            glm::vec2 uv = TextureUtils::cubeToUV(glm::vec4(last[0], last[1], last[2], 1));
            vector.insert(vector.end(), {uv[0], uv[1]}); // uv

            vector.insert(vector.end(), {second[0], second[1], second[2]});
            vector.insert(vector.end(), {normal[0], normal[1], normal[2]});
            uv = TextureUtils::cubeToUV(glm::vec4(second[0], second[1], second[2], 1));
            vector.insert(vector.end(), {uv[0], uv[1]}); // uv
        }
    }
    return vector;
}

void Cube::drawShape() {
    POS_XY->draw();
    NEG_XY->draw();
    POS_ZY->draw();
    NEG_ZY->draw();
    POS_XZ->draw();
    NEG_XZ->draw();
}
