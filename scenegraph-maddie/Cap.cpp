#include "Cap.h"
#include <vector>
#include "gl/shaders/Shader.h"
#include "gl/shaders/ShaderAttribLocations.h"

Cap::Cap(int multiplier, glm::vec3 normal) :
    m_multiplier(multiplier),
    m_normal(normal),
    CAP(nullptr)
{
}

Cap::~Cap() {
}

void Cap::createVertices(int p1, int p2) {
    std::vector<float> cap = buildCap(p1, p2);
    CAP = std::make_unique<OpenGLShape>();
    CAP->setVertexData(&cap[0], cap.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, cap.size()/6);
    CAP->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    CAP->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    CAP->buildVAO();
}

std::vector<float> Cap::buildCap(int p1, int p2) {
    float theta = 2*M_PI / p2;
    float chunk = 0.5 / p1;
    std::vector<float> face;
    std::vector<glm::vec3> points; // vertices around the cylinder
    for (int i = 0; i < p2; i++) {
        points.push_back(glm::vec3((0.5) * cos(m_multiplier * i * theta), m_multiplier * 0.5, (0.5) * sin(m_multiplier * i * theta)));
    }

    for (int j = 0; j < p1-1; j++) { // make one "ring" at each iter.
      // generate ring of points
      points.clear();
      for (int i = 0; i < p2; i++) {
          points.push_back(glm::vec3((0.5 - j * chunk) * cos(m_multiplier * i * theta), m_multiplier * 0.5, (0.5 - j * chunk) * sin(m_multiplier * i * theta)));
      }
      points.push_back(glm::vec3((0.5 - j * chunk) * cos(0), m_multiplier * 0.5, (0.5 - j * chunk) * sin(0))); // add first point again to complete ring

      // create ring
      for (int k = 0; k < points.size(); k++) { // 1 iter. per side of the donut
          glm::vec3 curr_point = glm::vec3(points[k][0], points[k][1], points[k][2]);
          glm::vec3 offset = glm::vec3(curr_point[0]/(p1-j), curr_point[1], curr_point[2]/(p1-j));

          face.insert(face.end(), {curr_point[0], curr_point[1], curr_point[2]});
          face.insert(face.end(), {m_normal[0], m_normal[1], m_normal[2]}); // normal
          face.insert(face.end(), {curr_point[0] - offset[0], curr_point[1], curr_point[2] - offset[2]});
          face.insert(face.end(), {m_normal[0], m_normal[1], m_normal[2]}); // normal
      }
    }

    // create "pizza slices"
    for (int l = 0; l < points.size(); l++) {
        face.insert(face.end(), {0.f, static_cast<float>(m_multiplier * 0.5), 0.f}); // center
        face.insert(face.end(), {m_normal[0], m_normal[1], m_normal[2]}); // normal

        face.insert(face.end(), {points[l][0], points[l][1], points[l][2]}); // outer
        face.insert(face.end(), {m_normal[0], m_normal[1], m_normal[2]}); // normal

        if (l+1 < points.size()) {
            face.insert(face.end(), {points[l+1][0], points[l+1][1], points[l+1][2]}); // next outer
            face.insert(face.end(), {m_normal[0], m_normal[1], m_normal[2]}); // normal
        } else {
            face.insert(face.end(), {points[0][0], points[0][1], points[0][2]});
            face.insert(face.end(), {m_normal[0], m_normal[1], m_normal[2]}); // normal
        }

        face.insert(face.end(), {0.f, static_cast<float>(m_multiplier * 0.5), 0.f}); // center
        face.insert(face.end(), {m_normal[0], m_normal[1], m_normal[2]}); // normal
    }
    return face;
}

void Cap::drawShape() {
    CAP->draw();
}
