#ifndef RAYSCENE_H
#define RAYSCENE_H

#include "Scene.h"
#include "Canvas2D.h"
#include <vector>


/**
 * @class RayScene
 *
 *  Students will implement this class as necessary in the Ray project.
 */
class RayScene : public Scene {
public:
    RayScene(Scene &scene);
    virtual ~RayScene();

    void paintScene(Canvas2D *canvas, Camera *camera, int width, int height);
    void settingsChanged() override;

private:
    glm::vec3 paintPoint(glm::vec4 pEye, glm::vec4 d);
    glm::vec3 paintPointRecursive(glm::vec4 pEye, glm::vec4 d, int iter);
    glm::vec3 getLighting(glm::vec4 point, glm::vec4 normal, ListNode primitive, glm::vec4 pEye);
    CS123RayIntersection getIntersection(glm::vec4 point, glm::vec4 direction, bool shadowCalculation=false);
    unsigned char REAL2byte(float f);
    const int limit = 4;

    // interface settings
    bool m_shadows;
    bool m_textureMap;
    bool m_reflection;
    bool m_point;
    bool m_directional;

};

#endif // RAYSCENE_H
