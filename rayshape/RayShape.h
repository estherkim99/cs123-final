#ifndef RAYSHAPE_H
#define RAYSHAPE_H

#include "glm/gtx/transform.hpp"
#include <cmath>
//#include "OpenGLScene.h"

class RayShape
{
public:
    RayShape();
    virtual ~RayShape();

    /** Given eye point and direction vector p, g in object coordinates,
     * return normal of the object if ray intersects **/
    virtual glm::vec4 getNormForIntersection(glm::vec4 p, glm::vec4 d) = 0;
    virtual glm::vec2 getUV(glm::vec4 p, glm::vec4 n) = 0;

};

#endif // RAYSHAPE_H
