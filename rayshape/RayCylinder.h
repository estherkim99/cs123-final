#ifndef RAYCYLINDER_H
#define RAYCYLINDER_H

#include "RayShape.h"

class RayCylinder : public RayShape
{
public:
    RayCylinder();
    virtual ~RayCylinder();

    virtual glm::vec4 getNormForIntersection(glm::vec4 p, glm::vec4 d);
    virtual glm::vec2 getUV(glm::vec4 p, glm::vec4 n);
};

#endif // RAYCYLINDER_H
