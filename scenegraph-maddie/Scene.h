#ifndef SCENE_H
#define SCENE_H

#include "CS123SceneData.h"
#include "ListNode.h"
#include <list>
#include "SupportCanvas2D.h"

class Camera;
class CS123ISceneParser;


/**
 * @class Scene
 *
 * @brief This is the base class for all scenes. Modify this class if you want to provide
 * common functionality to all your scenes.
 */
class Scene {
public:
    Scene();
    Scene(Scene &scene);
    virtual ~Scene();

    virtual void settingsChanged() {}

    static void parse(Scene *sceneToFill, CS123ISceneParser *parser);

protected:

    // Adds a primitive to the scene.
    virtual void addPrimitive(CS123ScenePrimitive scenePrimitive, const glm::mat4x4 matrix);

    // Adds a light to the scene.
    virtual void addLight(const CS123SceneLightData &sceneLight);

    // Sets the global data for the scene.
    virtual void setGlobal(const CS123SceneGlobalData &global);

    virtual void buildList(CS123SceneNode *root, glm::mat4x4 matrix);
    static glm::mat4x4 getTransformation(CS123SceneTransformation* t);

    void loadTextures(std::vector<ListNode> &m_nodes);

    std::vector<ListNode> m_nodes;
    std::vector<CS123SceneLightData> m_lights;
    CS123SceneGlobalData m_globalData;

};

#endif // SCENE_H
