#include "CS123Shader.h"
#include "CS123SceneData.h"
#include "Settings.h"
#include "glm/ext.hpp"
#include "gl/GLDebug.h"
#include <sstream>

namespace CS123 { namespace GL {


CS123Shader::CS123Shader(const std::string &vertexSource, const std::string &fragmentSource) :
    Shader(vertexSource, fragmentSource)
{
}

CS123Shader::CS123Shader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragmentSource) :
    Shader(vertexSource, geometrySource, fragmentSource)
{
}

glm::vec3 toGLMVec3(const CS123SceneColor &c) {
    return glm::vec3(c.r, c.g, c.b);
}

void CS123Shader::applyMaterial(const CS123SceneMaterial &material) {
    setUniform("ambient_color", toGLMVec3(material.cAmbient));
    setUniform("diffuse_color", toGLMVec3(material.cDiffuse));
    setUniform("specular_color", toGLMVec3(material.cSpecular));
    setUniform("shininess", material.shininess);
}

void CS123Shader::setLight(const CS123SceneLightData &light) {
    bool ignoreLight = false;

    GLint lightType;
    std::string name;
    glm::vec3 ndir;
    switch(light.type) {
        case LightType::LIGHT_POINT:
            lightType = 0;
            name = "lightPositions";
            setUniformArrayByIndex(name, light.pos.xyz(), light.id);
            if (!settings.usePointLights) ignoreLight = true;
            break;
        case LightType::LIGHT_DIRECTIONAL:
            lightType = 1;
            ndir = glm::normalize(light.dir.xyz());
            name = "lightDirections";
            setUniformArrayByIndex(name, ndir, light.id);
            if (!settings.useDirectionalLights) ignoreLight = true;
            break;
        default:
            lightType = 0;
            ignoreLight = true; // Light type not supported
            break;
    }

    CS123SceneColor color = light.color;
    if (ignoreLight) color.r = color.g = color.b = 0;

    setUniformArrayByIndex("lightTypes", lightType, light.id); // index is last
    setUniformArrayByIndex("lightColors", glm::vec3(color.r, color.g, color.b), light.id);
//    setUniformArrayByIndex("lightAttenuations", light.function, light.id);

    // set up a light space transformation matrix for each light id
    GLfloat near_plane = 1.0f, far_plane = 7.5f;;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3( 0.0f, 0.0f,  0.0f),  glm::vec3(0.0f, 1.0f, 0.0f));
//    glm::mat4 lightView = glm::lookAt(light.pos.xyz(), glm::vec3(0.0f),  glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    setUniformArrayByIndex("lightSpaceMatrix", lightSpaceMatrix, light.id);
}

}}
