#include "openglshape.h"

#include "gl/datatype/VAO.h"
#include "gl/shaders/ShaderAttribLocations.h"

using namespace CS123::GL;

OpenGLShape::OpenGLShape() :
    m_size(0),
    m_drawMode(VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLES),
    m_numVertices(0),
    m_VAO(nullptr)
{
}

OpenGLShape::~OpenGLShape()
{
}


/**
 * @param data - Vector of floats containing the vertex data.
 * @param size - number of elements in the array.
 * @param drawMode - Drawing mode
 * @param numVertices - Number of vertices to be rendered.
 */
void OpenGLShape::setVertexData(GLfloat *data, int size, VBO::GEOMETRY_LAYOUT drawMode, int numVertices) {
    // Store the vertex data and other values to be used later when constructing the VAO
    m_data = data;
    m_size = size;
    m_drawMode = drawMode;
    m_numVertices = numVertices;
}


/**
 * @param index OpenGL handle to the attribute location. These are specified in ShaderAttribLocations.h
 * @param numElementsPerVertex Number of elements per vertex. Must be 1, 2, 3 or 4 (e.g. position = 3 for x,y,z)
 * @param offset Offset in BYTES from the start of the array to the beginning of the first element
 * @param type Primitive type (FLOAT, INT, UNSIGNED_BYTE)
 * @param normalize
 */
void OpenGLShape::setAttribute(
        GLuint index, GLuint numElementsPerVertex, int offset, VBOAttribMarker::DATA_TYPE type, bool normalize) {
    m_markers.push_back(VBOAttribMarker(index, numElementsPerVertex, offset, type, normalize));
}

void OpenGLShape::buildVAO() {
    CS123::GL::VBO vbo = VBO(m_data, m_size, m_markers, m_drawMode);
    m_VAO = std::make_unique<VAO>(vbo, m_numVertices);
}

void OpenGLShape::draw() {
    if (m_VAO) {
        m_VAO->bind();
        m_VAO->draw();
        m_VAO->unbind();
    }
}

void OpenGLShape::initializeShape(int p1, int p2) {
    std::vector<float> vertices{};
    makeSides(&vertices, p1, p2);
    computeUV(&vertices);
    computeTangentsAndBinormals(&vertices);
    setData(&vertices);

}

void OpenGLShape::setData(std::vector<float>* vertices){
    int size = static_cast<int>(vertices->size());
    this->setVertexData(vertices->data(), size, VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, size / DATAPERVERTEX);
    this->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    this->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    this->setAttribute(ShaderAttrib::TEXCOORD0, 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    this->setAttribute(ShaderAttrib::TANGENT, 3, 32, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    this->buildVAO();
}

void OpenGLShape::rotateSurface(std::vector<float>* target, glm::vec3 dir, float angle, int dataNum) {
    std::vector<float> side{};
    side.resize(dataNum);
    for (int i = 0; i < dataNum; i) {
        glm::vec4 pos = glm::rotate(angle, dir) * (glm::vec4(target->at(i), target->at(i + 1), target->at(i + 2), 1.0));
        side.at(i++) = pos.x;
        side.at(i++) = pos.y;
        side.at(i++) = pos.z;
        glm::vec4 norm = glm::rotate(angle, dir) * glm::vec4(target->at(i), target->at(i + 1), target->at(i + 2), 0.0);
        side.at(i++) = norm.x;
        side.at(i++) = norm.y;
        side.at(i++) = norm.z;
        // texcoords, tangent
        for (int j = 0; j < DATAPERVERTEX - 6; j++) {
            side.at(i++) = 0;
        }
    }
    target->insert(target->end(), side.begin(), side.end());
}

void OpenGLShape::computeUV(std::vector<float> *vertices){
    for (int i = 0; i < vertices->size(); i += DATAPERVERTEX) {
        glm::vec4 pos = glm::vec4(vertices->at(i), vertices->at(i+1), vertices->at(i+2), 1.f);
        glm::vec2 uv = getUVfromPosition(pos);
        vertices->at(i+6) = uv.x;
        vertices->at(i+7) = uv.y;
    }
}


void OpenGLShape::computeTangentsAndBinormals(std::vector<float> *vertices){
    for (int i = 0; i <= vertices->size() - 3 * DATAPERVERTEX; i+= DATAPERVERTEX) {
        glm::vec3 v0 = glm::vec3(vertices->at(i), vertices->at(i+1), vertices->at(i+2));
        glm::vec3 v1 = glm::vec3(vertices->at(i+DATAPERVERTEX), vertices->at(i+1 + DATAPERVERTEX), vertices->at(i+2 + DATAPERVERTEX));
        glm::vec3 v2 = glm::vec3(vertices->at(i+2 * DATAPERVERTEX), vertices->at(i+1 + 2*DATAPERVERTEX), vertices->at(i+2 +2*DATAPERVERTEX));

        glm::vec2 uv0 = glm::vec2(vertices->at(i+6), vertices->at(i+7));
        glm::vec2 uv1 = glm::vec2(vertices->at(i+6 + DATAPERVERTEX), vertices->at(i+ 7 + DATAPERVERTEX));
        glm::vec2 uv2 = glm::vec2(vertices->at(i+6 + 2 * DATAPERVERTEX), vertices->at(i+7 + 2 * DATAPERVERTEX));

        glm::vec3 deltaPos1 = v1-v0;
        glm::vec3 deltaPos2 = v2-v0;

        glm::vec2 deltaUV1 = uv1-uv0;
        glm::vec2 deltaUV2 = uv2-uv0;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
        int limit = 1;
//        if (i == vertices->size() - 3 * DATAPERVERTEX) {
//            limit = 3;
//        }
        for (int j = 0; j < limit; j++) {
            vertices->at(i+8 + j * DATAPERVERTEX) = tangent.x;
            vertices->at(i+9 + j * DATAPERVERTEX) = tangent.y;
            vertices->at(i+10 + j * DATAPERVERTEX) = tangent.z;
        }
    }

}
