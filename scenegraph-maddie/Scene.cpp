#include "Scene.h"
#include "Camera.h"
#include "CS123ISceneParser.h"
#include "glm/gtx/transform.hpp"
#include <iostream>
#include <QFileInfo>
#include <QImage>

Scene::Scene()
{
    m_nodes = std::vector<ListNode>();
    m_lights = std::vector<CS123SceneLightData>();
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
    m_nodes = scene.m_nodes;
    m_lights = scene.m_lights;
    m_globalData = scene.m_globalData;
}

Scene::~Scene()
{
    // Do not delete m_camera, it is owned by SupportCanvas3D
}

void Scene::parse(Scene *sceneToFill, CS123ISceneParser *parser) {
    // TODO: load scene into sceneToFill using setGlobal(), addLight(), addPrimitive(), and finishParsing()
    CS123SceneGlobalData globalData;
    parser->getGlobalData(globalData);
    sceneToFill->setGlobal(globalData);

    for (int i = 0; i < parser->getNumLights(); i++) {
        CS123SceneLightData lightData;
        parser->getLightData(i, lightData);
        sceneToFill->addLight(lightData);
    }

    glm::mat4x4 mat;
    sceneToFill->buildList(parser->getRootNode(), mat);
}

/**
 * @brief Scene::buildList: builds list of nodes to be drawn, including their
 * @param root
 * @param matrix: any composed matrix from farther up in the graph that must be applied
 * @return
 */
void Scene::buildList(CS123SceneNode *root, glm::mat4x4 matrix) {
    // compose transformations
    std::vector<CS123SceneTransformation*> transformations = root->transformations;
    for (int i = 0; i < transformations.size(); i++) {
        glm::mat4x4 transformation = getTransformation(transformations[i]);
        matrix = matrix * transformation;
    }

    // add primitive + composed transformation to the scene
    std::vector<CS123ScenePrimitive*> primitives = root->primitives;
    for (int i = 0; i < primitives.size(); i++) {
        CS123ScenePrimitive *p = primitives[i];
        addPrimitive(*p, matrix);
    }

    // if children exist, recurse
    std::vector<CS123SceneNode*> children = root->children;
    for (int i = 0; i < children.size(); i++) {
        buildList(children[i], matrix);
    }
}

glm::mat4x4 Scene::getTransformation(CS123SceneTransformation* t) {
    switch (t->type) {
        case TRANSFORMATION_ROTATE:
            return glm::rotate(t->angle, t->rotate);
        case TRANSFORMATION_SCALE:
            return glm::scale(t->scale);
        case TRANSFORMATION_TRANSLATE:
            return glm::translate(t->translate);
        case TRANSFORMATION_MATRIX:
            return t->matrix;
    }
}

void Scene::addPrimitive(CS123ScenePrimitive scenePrimitive, const glm::mat4x4 matrix) {
    // apply global lighting constants before adding node
    scenePrimitive.material.cAmbient *= m_globalData.ka;
    scenePrimitive.material.cDiffuse *= m_globalData.kd;
    scenePrimitive.material.cSpecular *= m_globalData.ks;
    m_nodes.push_back(ListNode(scenePrimitive, matrix));
}

void Scene::addLight(const CS123SceneLightData &sceneLight) {
    m_lights.push_back(sceneLight);
}

void Scene::setGlobal(const CS123SceneGlobalData &global) {
    m_globalData = global;
}

void Scene::loadTextures(std::vector<ListNode> &m_nodes) {
    for (int i = 0; i < m_nodes.size(); i++) {
        ListNode &currNode = m_nodes[i];

        if (currNode.m_object.material.textureMap.isUsed) {
            CS123SceneFileMap textureMap = currNode.m_object.material.textureMap;
            currNode.m_texture = QImage::QImage(QString::fromStdString(textureMap.filename));
        }
    }
}
