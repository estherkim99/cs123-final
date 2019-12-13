#include "Scene.h"
#include "Camera.h"
#include "CS123ISceneParser.h"
#include "Settings.h"
#include "glm/gtx/transform.hpp"
#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"
#include "gl/textures/Texture2D.h"

Scene::Scene() :
    m_id(0)
{
}

Scene::Scene(Scene &scene)
{
    // We need to set the global constants to one when we duplicate a scene,
    // otherwise the global constants will be double counted (squared)
    CS123SceneGlobalData global = { 1, 1, 1, 1};
    setGlobal(global);

    // TODO [INTERSECT]
    // Make sure to copy over the lights and the scenegraph from the old scene,
    // as well as any other member variables your new scene will need.

    m_lightData = scene.m_lightData;
    m_sceneObjects = scene.m_sceneObjects;
    m_ka = scene.m_ka;
    m_kd = scene.m_kd;
    m_ks = scene.m_ks;
    m_kt = scene.m_kt;
}

Scene::~Scene()
{
    // Do not delete m_camera, it is owned by SupportCanvas3D
}

void Scene::parse(Scene *sceneToFill, CS123ISceneParser *parser) {
    // TODO: load scene into sceneToFill using setGlobal(), addLight(), addPrimitive()
    // set global
    CS123SceneGlobalData global;
    parser->getGlobalData(global);
    sceneToFill->setGlobal(global);
    // add light
    CS123SceneLightData light;
    for (int i = 0; i < parser->getNumLights(); i++) {
        parser->getLightData(i, light);
        sceneToFill->addLight(light);
    }
    // addPrimitive();
    CS123SceneNode *node = parser->getRootNode();
    glm::mat4 composite = glm::mat4();
    sceneToFill->parseNode(sceneToFill, node, composite);
    sceneToFill->loadTextureImages();
}

void Scene::parseNode(Scene *sceneToFill, CS123SceneNode *node, glm::mat4x4 composite) {
    std::vector<CS123SceneTransformation *> t = node->transformations;
    for (int i = 0; i < t.size(); i++) {
        CS123SceneTransformation *x = t[i];
        switch (x->type){
        case TRANSFORMATION_TRANSLATE :
            composite = glm::translate(composite, x->translate);
            break;
        case TRANSFORMATION_ROTATE :
            composite = glm::rotate(composite, x->angle, x->rotate);
            break;
        case TRANSFORMATION_SCALE :
            composite = glm::scale(composite, x->scale);
            break;
        case TRANSFORMATION_MATRIX :
            composite *= x->matrix;
            break;
        }
    }
    std::vector<CS123ScenePrimitive *> p = node->primitives;

    for (int i = 0; i < p.size(); i++) {
        sceneToFill->addPrimitive(*p[i], composite);
    }
    std::vector<CS123SceneNode *> c = node->children;
    for (int i = 0; i < c.size(); i++) {
        sceneToFill->parseNode(sceneToFill, c[i], composite);
    }
}

void Scene::addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix) {
    SceneObject obj;
    CS123SceneMaterial m;
    obj.id = m_id++;
    obj.material = scenePrimitive.material;
    obj.composite = matrix;
    obj.invMat = glm::inverse(matrix);
    obj.primitive = scenePrimitive.type;
    m_sceneObjects.push_back(obj);
}

void Scene::addLight(const CS123SceneLightData &sceneLight) {
    m_lightData.push_back(sceneLight);
}

void Scene::setGlobal(const CS123SceneGlobalData &global) {
    m_ka = global.ka;
    m_kd = global.kd;
    m_ks = global.ks;
    m_kt = global.kt;
}

void Scene::loadTextureImages() {
    if (settings.useTextureMapping) {
        m_textures.resize(m_sceneObjects.size());
        for (SceneObject x : m_sceneObjects) {
            if (x.material.textureMap.isUsed) {
                QString fname = QString::fromStdString(x.material.textureMap.filename);
                QImage texture(fname);
                QImage convertedImage = QGLWidget::convertToGLFormat(texture);
                // m_textures.at(x.id) = texture;
                m_textures.at(x.id) = convertedImage;
            }
        }
    }
}
