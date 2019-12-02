#include "RayScene.h"
#include "Settings.h"
#include "CS123SceneData.h"
#include "intersect/IntersectUtils.h"
#include "intersect/TextureUtils.h"
#include "Canvas2D.h"
#include "Camera.h"
#include <algorithm>
#include <iostream>
#include "Settings.h"

RayScene::RayScene(Scene &scene) :
    Scene(scene)
{
    // TODO [INTERSECT]
    // Remember that any pointers or OpenGL objects (e.g. texture IDs) will
    // be deleted when the old scene is deleted (assuming you are managing
    // all your memory properly to prevent memory leaks).  As a result, you
    // may need to re-allocate some things here.
    m_shadows = settings.useShadows;
    m_textureMap = settings.useTextureMapping;
    m_reflection = settings.useReflection;
    m_point = settings.usePointLights;
    m_directional = settings.useDirectionalLights;
}

RayScene::~RayScene()
{
}

/**
 * @brief RayScene::paintScene - generates rays and finds shape intersections with them
 * @param canvas
 * @param camera
 * @param width
 * @param height
 */
void RayScene::paintScene(Canvas2D *canvas, Camera *camera, int width, int height) {
    glm::mat4x4 filmToWorld = glm::inverse(camera->getViewMatrix()) * glm::inverse(camera->getScaleMatrix());
    glm::vec4 pEye = glm::vec4(0.f, 0.f, 0.f, 1.f);
    pEye = filmToWorld * pEye; // pEye in world space

    if (m_textureMap) { // load in textures if texture mapping turned on
        loadTextures(m_nodes);
    }

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            glm::vec4 pFilm = glm::vec4(2.f*i/width - 1.f, 1.f - (2.f*j/height), -1.f, 1.f); // convert screen space to pixel on film plane
            glm::vec4 pWorld = filmToWorld * pFilm; // pWorld in world space
            glm::vec4 d = glm::normalize(pWorld - pEye); // d in world space
            glm::vec3 color;

            if (m_reflection) {
                color = paintPointRecursive(pEye, d, 0);
            } else {
                color = paintPoint(pEye, d);
            }
            canvas->data()[j*width+i] = RGBA(REAL2byte(color.x), REAL2byte(color.y), REAL2byte(color.z));
        }
    }
    canvas->update();
}

// pEye, d in world space
glm::vec3 RayScene::paintPointRecursive(glm::vec4 pEye, glm::vec4 d, int iter) {
    if (iter > limit) { // do not recurse if limit is reached
        return glm::vec3(0, 0, 0);
    }

    CS123RayIntersection intersection = getIntersection(pEye, d); // intersection with closest primitive
    if (intersection.t == std::numeric_limits<float>::max()) { // if no intersection, paint canvas black
        return glm::vec3(0, 0, 0);
    }

    // otherwise, ray trace recursively
    ListNode primitive = m_nodes[intersection.i];
    glm::vec4 intersectionPoint = pEye + intersection.t*d;
    glm::vec4 normal = glm::normalize(glm::vec4(glm::transpose(glm::inverse(glm::mat3x3(primitive.m_transformation))) * glm::vec3(intersection.normal), 0)); // translate object space normal into world space
    glm::vec4 reflection = glm::normalize(glm::reflect(d, normal));

    return getLighting(intersectionPoint, intersection.normal, primitive, pEye) +
            glm::vec3(m_globalData.ks * primitive.m_object.material.cReflective.r,
                      m_globalData.ks * primitive.m_object.material.cReflective.g,
                      m_globalData.ks * primitive.m_object.material.cReflective.b) *
            paintPointRecursive(intersectionPoint + 0.001f*normal, reflection, iter+1);
}

glm::vec3 RayScene::paintPoint(glm::vec4 pEye, glm::vec4 d) {
    CS123RayIntersection intersection = getIntersection(pEye, d); // intersection with closest primitive
    if (intersection.t == std::numeric_limits<float>::max()) { // if no intersection, paint canvas black
        return glm::vec3(0, 0, 0);
    } else {
        return getLighting(pEye+intersection.t*d, intersection.normal, m_nodes[intersection.i], pEye);
    }
}

// point and dir are in world space, put into object space by IntersectUtils
// normal that's returned is in object space
CS123RayIntersection RayScene::getIntersection(glm::vec4 point, glm::vec4 dir, bool shadowCalculation) {
    float closestT = std::numeric_limits<float>::max();
    CS123RayIntersection output = {-1, std::numeric_limits<float>::max(), glm::vec4(0, 0, 0, 0)}; // IS IT A PROBLEM TO RETURN THIS? since it's on the stack??

    for (int i = 0; i < m_nodes.size(); i++)  {
        ListNode currNode = m_nodes[i];
        CS123RayIntersection info = {-1, std::numeric_limits<float>::max(), glm::vec4(0, 0, 0, 0)};

        switch (currNode.m_object.type) {
            case PrimitiveType::PRIMITIVE_CUBE: {
                info = IntersectUtils::findSquareIntersection(i, point, dir, currNode);
                break;
            }
            case PrimitiveType::PRIMITIVE_CYLINDER: {
                info = IntersectUtils::findCylinderIntersection(i, point, dir, currNode);
                break;
            }
            case PrimitiveType::PRIMITIVE_CONE: {
                info = IntersectUtils::findConeIntersection(i, point, dir, currNode);
                break;
            }
            case PrimitiveType::PRIMITIVE_SPHERE: {
                info = IntersectUtils::findSphereIntersection(i, point, dir, currNode);
                break;
            }
            // un-implemented shapes
            case PrimitiveType::PRIMITIVE_TORUS:
                break;
            case PrimitiveType::PRIMITIVE_MESH:
                break;
        }

        if (info.t >= 0.f && info.t < closestT) {
            closestT = info.t;
            output = {i, info.t, info.normal}; // normal in object space

            if (shadowCalculation) { // if calculating shadows, exit early (any intersection means the object is occluded)
                break;
            }
        }
    }
    return output;
}

/**
 * @brief RayScene::getLighting gets lighting of point from all lights in the scene
 * @param point in world space
 * @param normal in object space
 * @param primitive
 * @return
 */
glm::vec3 RayScene::getLighting(glm::vec4 point, glm::vec4 normal, ListNode primitive, glm::vec4 pEye) {
    normal = glm::normalize(glm::vec4(glm::transpose(glm::inverse(glm::mat3x3(primitive.m_transformation))) * glm::vec3(normal), 0)); // translate object space normal into world space
    float ambientR = primitive.m_object.material.cAmbient.r;
    float ambientG = primitive.m_object.material.cAmbient.g;
    float ambientB = primitive.m_object.material.cAmbient.b;

    float diffuseR = 0.f;
    float diffuseG = 0.f;
    float diffuseB = 0.f;

    float specularR = 0.f;
    float specularG = 0.f;
    float specularB = 0.f;

    glm::vec3 texColor = glm::vec3(0, 0, 0);
    float blend = 0.f;
    if (m_textureMap && !primitive.m_texture.isNull()) {
        texColor = TextureUtils::blendTexture(point, primitive);
        blend = primitive.m_object.material.blend;
    }

    // calculate lighting at point
    for (int i = 0; i < m_lights.size(); i++) {
        CS123SceneLightData currLight = m_lights[i];
        glm::vec4 light; // from intersection to light
        float atten;

        if (currLight.type == LightType::LIGHT_DIRECTIONAL) {
            if (!m_directional) { // if directional lights not checked in UI, skip
                continue;
            }
            light = glm::normalize(-1.f*currLight.dir);
            atten = 1.f;
        } else {
            if (!m_point) { // if point lights not checked in UI, skip
                continue;
            }
            light = glm::normalize(currLight.pos - point);
            float distance = glm::distance(currLight.pos, point);
            atten = std::min(1.f/(currLight.function.x + currLight.function.y*distance + currLight.function.z*(float)pow(distance, 2)), 1.f);
        }

        if (m_shadows) { // if an object occludes this light, skip it
            CS123RayIntersection intersection = getIntersection(point + 0.001f*normal, light, true);
            if (intersection.t != std::numeric_limits<float>::max()) {
                continue;
            }
        }

        glm::vec4 v = glm::normalize(pEye - point); // from light to intersection
        glm::vec4 reflection = glm::normalize(glm::reflect(-light, glm::normalize(normal)));

        diffuseR += atten * (float)currLight.color.r * (blend * texColor.r + (1.f-blend) * ((float)primitive.m_object.material.cDiffuse.r)) * glm::max(glm::dot(normal, light), 0.f);
        specularR += atten * (float)currLight.color.r * (float)primitive.m_object.material.cSpecular.r * pow(glm::max(glm::dot(reflection, v), 0.f), primitive.m_object.material.shininess);

        diffuseG += atten * (float)currLight.color.g * (blend * texColor.g + (1.f-blend) * ((float)primitive.m_object.material.cDiffuse.g)) * glm::max(glm::dot(normal, light), 0.f);
        specularG += atten * (float)currLight.color.r * (float)primitive.m_object.material.cSpecular.g * pow(glm::max(glm::dot(reflection, v), 0.f), primitive.m_object.material.shininess);

        diffuseB += atten * (float)currLight.color.b * (blend * texColor.b + (1.f-blend) * ((float)primitive.m_object.material.cDiffuse.b)) * glm::max(glm::dot(normal, light), 0.f);
        specularB += atten * (float)currLight.color.r * (float)primitive.m_object.material.cSpecular.b * pow(glm::max(glm::dot(reflection, v), 0.f), primitive.m_object.material.shininess);
    }

    return glm::vec3(ambientR + diffuseR + specularR,
                     ambientG + diffuseG + specularG,
                     ambientB + diffuseB + specularB);
}

void RayScene::settingsChanged() {
    m_shadows = settings.useShadows;
    m_textureMap = settings.useTextureMapping;
    m_reflection = settings.useReflection;
    m_point = settings.usePointLights;
    m_directional = settings.useDirectionalLights;
}

unsigned char RayScene::REAL2byte(float f) {
    int i = static_cast<int>((f * 255.0 + 0.5));
    return (i < 0) ? 0 : (i > 255) ? 255 : i;
}
