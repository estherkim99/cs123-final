#include "RayScene.h"
#include "Settings.h"
#include "CS123SceneData.h"

#include <iostream>
#include "Camera.h"
#include <QApplication>

#define RAY_RECURSION_DEPTH 3

RayScene::RayScene(Scene &scene) :
    Scene(scene),
    m_rayshape(nullptr)
{
    initializeShapes();
}

void RayScene::initializeShapes(){
    m_rayCube = std::make_shared<RayCube>();
    m_rayCone = std::make_shared<RayCone>();
    m_rayCylinder = std::make_shared<RayCylinder>();
    m_raySphere = std::make_shared<RaySphere>();
}

void RayScene::rayTrace(Camera *camera, Canvas2D *canvas){

    updateFilmToWorldMatrix(camera); // load information from camera

    if (settings.useTextureMapping) {
        loadTextures();
    }

    int width = canvas->width();
    int height = canvas->height();
    for (int y = 0 ; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pt = y * width + x;
            glm::vec4 d_w = generateRays(x, y, width, height);
            m_depth = 0;
            glm::vec3 color = checkIntersect(m_eye, d_w);
            // multiply by 255
            color*= 255.f;
            // clamp values to 0 - 255
            canvas->data()[pt].r = std::min(255.f, std::max(0.f, color.r));
            canvas->data()[pt].b = std::min(255.f, std::max(0.f, color.b));
            canvas->data()[pt].g = std::min(255.f, std::max(0.f, color.g));
            canvas->data()[pt].a = 255.f;
        }
        canvas->update();
        QApplication::processEvents();
    }
}

void RayScene::updateFilmToWorldMatrix(Camera *camera){
    m_filmToWorld = glm::inverse(camera->getViewMatrix()) * glm::inverse(camera->getScaleMatrix());
    m_eye = glm::inverse(camera->getViewMatrix()) * glm::vec4(0, 0, 0, 1);
}

glm::vec4 RayScene::generateRays(int x, int y, int xmax, int ymax){
    glm::vec4 p_film = glm::vec4(2.f * x / xmax - 1, 1 - (2.f * y) / ymax, -1, 1);
    glm::vec4 p_w = m_filmToWorld * p_film;
    glm::vec4 d_w = glm::normalize(p_w - m_eye);
    return d_w;
}


glm::vec3 RayScene::checkIntersect(glm::vec4 o_w, glm::vec4 d_w) {
    if (m_depth > RAY_RECURSION_DEPTH) {
        return glm::vec3();
    }

    glm::vec4 t = glm::vec4(0.f, 0.f, 0.f, std::numeric_limits<float>::infinity());
    int index = -1;
    // iterate through scene objects
    for (int i = 0; i < static_cast<int>(m_sceneObjects.size()); i++) {
        SceneObject o = m_sceneObjects.at(static_cast<unsigned long>(i));
        glm::vec4 p_object = o.invMat * o_w;
        glm::vec4 d_object = o.invMat * d_w;
        matchPrimitiveType(o);
        if (m_rayshape) {
            glm::vec4 normal = m_rayshape->getNormForIntersection(p_object, d_object);
            if (normal.w < t.w && (abs(normal.w) > 0.00001f)) {
                t = normal;
                index = i;
            }
        }
    }


    if (t.w < std::numeric_limits<float>::infinity()) { // intersects
        if (m_shadow) {
            return glm::vec3(1.f);
        }
        SceneObject o = m_sceneObjects.at(static_cast<unsigned long>(index));
        glm::vec4 p_w = o_w + t.w * d_w; // intersection point

        // get normal
        glm::mat3x3 objectToWorld = glm::transpose(glm::mat3(o.invMat));
        glm::vec4 n_w = glm::vec4(glm::normalize(objectToWorld * glm::normalize(glm::vec3(t))), 0);
        // get uv
        matchPrimitiveType(o);
        if (m_rayshape) {
            glm::vec2 uv = m_rayshape->getUV(o.invMat * p_w, t);
            return lightEq(&o.material, p_w, d_w, n_w, uv, o.id);
        }
    }
    // does not intersect
    return glm::vec3();
}


glm::vec3 RayScene::lightEq(CS123SceneMaterial *mat, glm::vec4 p, glm::vec4 d, glm::vec4 n, glm::vec2 uv, int id) {
    CS123SceneLightData *light;
    unsigned long num_lights = m_lightData.size();
    glm::vec3 color = glm::vec3(0.f);



    for (unsigned long i = 0; i < num_lights; i++) {
        light = &m_lightData.at(i);
        glm::vec4 L = glm::vec4();
        if (light->type == LightType::LIGHT_POINT && settings.usePointLights) {
            L = light->pos - p;
        } else if (light->type == LightType::LIGHT_DIRECTIONAL && settings.useDirectionalLights) {
            L = -light->dir;
        }

        L.w = 0.f;
        float dst = glm::length(L);
        L = glm::normalize(L);
        if (!settings.useShadows || !shadowed(L, p)) {
            // get attenuation, diffuse, specular, reflections
            glm::vec3 attenuation = getAttenuation(light, dst);
            glm::vec3 diffuse = getDiffuse(mat, L, n, uv, id);
            glm::vec3 specular = getSpecular(mat, L, n, d);


            // add attenuation * (diffuse, specular)
            color.r += attenuation.r * (diffuse.r + specular.r);
            color.g += attenuation.g * (diffuse.g + specular.g);
            color.b += attenuation.b * (diffuse.b + specular.b);
        }
    }

    // add reflections
    if (settings.useReflection) {
        glm::vec3 reflections = getReflections(mat, n, -d, p);
        color += reflections;
    }

    // add ambient
    color.r += mat->cAmbient.r * m_ka;
    color.g += mat->cAmbient.g * m_ka;
    color.b += mat->cAmbient.b * m_ka;



    return color;
}

glm::vec3 RayScene::getDiffuse(CS123SceneMaterial *material, glm::vec4 L, glm::vec4 n, glm::vec2 uv, int id){
    // material color
    float r = material->cDiffuse.r * m_kd;
    float g = material->cDiffuse.g * m_kd;
    float b = material->cDiffuse.b * m_kd;

    // if texture-mapped
    if (settings.useTextureMapping && material->textureMap.isUsed)
    {
        // get texture
        QImage texture = m_textures.at(id);

        if (!texture.isNull()) {
            // get (w,h) - texture dimensions
            int w = texture.width();
            int h = texture.height();
            // get (j,k) - repeat val
            float j = material->textureMap.repeatU;
            float k = material->textureMap.repeatV;
            // get (s,t) - texure lookup coordinates
            int s = (static_cast<int>(uv.x * j * w)) % w;
            int t = (static_cast<int>(uv.y * k * h)) % h;
            QColor color_t(texture.pixel(s,t));
            // blending equation
            float blend = material->blend;
            r = (blend * color_t.red() / 255.f) + ((1.f - blend) * r);
            g = (blend * color_t.green() / 255.f) + ((1.f - blend) * g);
            b = (blend * color_t.blue() / 255.f) + ((1.f - blend) * b);
        }

    }

    float nL = std::max(0.f, glm::dot(n, L));
    return glm::vec3(r, g, b) * nL;
}

glm::vec3 RayScene::getAttenuation(CS123SceneLightData *light, float dst){
    glm::vec3 att = glm::vec3(light->color.r, light->color.g, light->color.b);
    glm::vec3 func = light->function;
    if (light->type == LightType::LIGHT_POINT) {
        att *= std::min(1.f, 1.f / (func.x + func.y * dst + func.z * dst * dst));
    } else if (light->type == LightType::LIGHT_DIRECTIONAL) {
        att *= 1.f;
    } else {
        att *= 0.f;
    }
    return att;
}

glm::vec3 RayScene::getSpecular(CS123SceneMaterial *material, glm::vec4 L,glm::vec4 n, glm::vec4 v){
    glm::vec4 R = glm::normalize(2.f * n * std::max(0.f, glm::dot(n, L)) - L);
    float reflectance = powf(std::max(glm::dot(R, -v), 0.f), material->shininess);
    glm::vec3 spec = glm::vec3(material->cSpecular.r, material->cSpecular.g, material->cSpecular.b);
    spec *= reflectance * m_ks;

    return glm::max(glm::vec3(0.f), spec);
}

glm::vec3 RayScene::getReflections(CS123SceneMaterial *material, glm::vec4 n, glm::vec4 d, glm::vec4 p){
    d = glm::normalize(d);
    p += p * 0.01f;
    glm::vec4 R = glm::normalize(2.f * n * std::max(0.f, glm::dot(n, d)) - d);
    m_depth++;
    glm::vec3 reflections = checkIntersect(p, R);
    reflections.r *= material->cReflective.r * m_ks;
    reflections.g *= material->cReflective.g * m_ks;
    reflections.b *= material->cReflective.b * m_ks;
    return glm::max(glm::vec3(), glm::min(glm::vec3(1.f), reflections));
}


void RayScene::matchPrimitiveType(SceneObject obj){
    m_rayshape = nullptr;
    switch (obj.primitive) {
    case PrimitiveType::PRIMITIVE_CUBE :
        m_rayshape = m_rayCube;
        break;
    case PrimitiveType::PRIMITIVE_CONE :
        m_rayshape = m_rayCone;
        break;
    case PrimitiveType::PRIMITIVE_CYLINDER :
        m_rayshape = m_rayCylinder;
        break;
    case PrimitiveType::PRIMITIVE_SPHERE :
        m_rayshape = m_raySphere;
        break;
    case PrimitiveType::PRIMITIVE_TORUS :
        break;
    case PrimitiveType::PRIMITIVE_MESH :
        break;
    }
}

bool RayScene::shadowed(glm::vec4 lightdir, glm::vec4 p){
    p += p * 0.01f;
    m_shadow = true;
    glm::vec3 reflections = checkIntersect(p, lightdir);
    if (reflections.x == 0.f && reflections.y == 0.f && reflections.z == 0.f) {
        m_shadow = false;
        return false;
    } else {
        m_shadow = false;
         return true;
    }

}



RayScene::~RayScene()
{

}

