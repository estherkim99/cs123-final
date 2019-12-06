#ifndef RAYSCENE_H
#define RAYSCENE_H

#include "Scene.h"

#include <vector>
#include "Canvas2D.h"
#include "rayshape/RayCube.h"
#include "rayshape/RayCone.h"
#include "rayshape/RayCylinder.h"
#include "rayshape/RaySphere.h"


class Canvas2D;


/**
 * @class RayScene
 *
 *  Students will implement this class as necessary in the Ray project.
 */
class RayScene : public Scene {
public:
    RayScene(Scene &scene);
    virtual ~RayScene();
    void rayTrace(Camera *camera, Canvas2D *canvas);


private:
    glm::vec4 generateRays(int x, int y, int width, int height);
    glm::vec3 checkIntersect(glm::vec4 o_w, glm::vec4 d_w);
    glm::vec3 lightEq(CS123SceneMaterial *material, glm::vec4 p, glm::vec4 d_w, glm::vec4 n, glm::vec2 uv, int id);
    void updateFilmToWorldMatrix(Camera *camera);
    void matchPrimitiveType(SceneObject obj);
    void initializeShapes();

    /**
     * @brief getAttenuation - calculates attenuattion component of lighting model
     * @param light - light i
     * @param mag - magnitude of nL
     * @return
     */
    glm::vec3 getAttenuation(CS123SceneLightData *light, float mag);

    /**
     * @brief getDiffuse - calculates diffuse component of lighting model
     * @param material
     * @param L
     * @param n
     * @param uv
     * @return
     */
    glm::vec3 getDiffuse(CS123SceneMaterial *material, glm::vec4 L, glm::vec4 n, glm::vec2 uv, int id);

    /**
     * @brief getSpecular - calculates specular component of lighting model
     * @param material - pointer to the material of intersected object
     * @param L - normalized vector from the intersection to light i
     * @param n - normalized normal to object at the point of intersection.
     * @param v - normalized line of sight
     * @return
     */
    glm::vec3 getSpecular(CS123SceneMaterial *material, glm::vec4 L, glm::vec4 n, glm::vec4 v);

    glm::vec3 getReflections(CS123SceneMaterial *material, glm::vec4 n, glm::vec4 d, glm::vec4 p);
    bool shadowed(glm::vec4 lightdir, glm::vec4 p);

    // member variables
    std::shared_ptr<RayShape> m_rayshape;
    std::shared_ptr<RayShape> m_rayCube;
    std::shared_ptr<RayShape> m_rayCone;
    std::shared_ptr<RayShape> m_rayCylinder;
    std::shared_ptr<RayShape> m_raySphere;

    glm::mat4x4 m_filmToWorld;
    glm::vec4 m_eye;
    int m_depth;
    bool m_shadow;
};

#endif // RAYSCENE_H
