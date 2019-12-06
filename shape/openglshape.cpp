#include "openglshape.h"

#include "gl/datatype/VAO.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "cubeshape.h"

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
    setData(&vertices);
}

//void OpenGLShape::makeSides(std::vector<float>* vertices, int p1, int p2) {
//}

void OpenGLShape::setData(std::vector<float>* vertices){
    int dataPerVertex = 6; // TODO: change to 8
    int size = static_cast<int>(vertices->size());
    this->setVertexData(vertices->data(), size, VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, size / dataPerVertex);
    this->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    this->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    // TODO: add line below when UV coordinates are added
    // this->setAttribute(ShaderAttrib::TEXCOORD0, 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false); // uv coords
    this->buildVAO();
}

void OpenGLShape::rotateSurface(std::vector<float>* target, glm::vec3 dir, float angle, int dataNum) {
    std::vector<float> side{};
    side.resize(dataNum);
    for (int i = 0; i < dataNum; i++) {
        glm::vec4 coord = glm::rotate(angle, dir) * (glm::vec4(target->at(i), target->at(i + 1), target->at(i + 2), 1.0));
        side.at(i++) = coord.x;
        side.at(i++) = coord.y;
        side.at(i++) = coord.z;
        glm::vec4 norm = glm::rotate(angle, dir) * glm::vec4(target->at(i), target->at(i + 1), target->at(i + 2), 0.0);
        side.at(i++) = norm.x;
        side.at(i++) = norm.y;
        side.at(i) = norm.z; // was just i before
        // add uv coords
        glm::vec2 uv = CubeShape::getUV(coord, norm);
//        side.at(i++) = uv.x;
//        side.at(i) = uv.y;
    }
    target->insert(target->end(), side.begin(), side.end());
}
