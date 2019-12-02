#ifndef SCENE_H
#define SCENE_H

#include "CS123SceneData.h"

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
    virtual void addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix);

    // Adds a light to the scene.
    virtual void addLight(const CS123SceneLightData &sceneLight);

    // Sets the global data for the scene.
    virtual void setGlobal(const CS123SceneGlobalData &global);

    struct SceneObject {
        int id;
        glm::mat4x4 composite; // transformation matrix
        glm::mat4x4 invMat; // inverse of transformation matrix
        CS123SceneMaterial material;
        PrimitiveType primitive; // object type

    };

    std::vector<CS123SceneLightData> m_lightData;
    std::vector<SceneObject> m_sceneObjects;
    float m_ka;
    float m_kd;
    float m_ks;
    float m_kt;


private:
    void parseNode(Scene *sceneToFill, CS123SceneNode *node, glm::mat4x4 composite);
    int m_id;

};

#endif // SCENE_H
