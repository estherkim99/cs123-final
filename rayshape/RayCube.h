#ifndef RAYCUBE_H
#define RAYCUBE_H

#include "RayShape.h"

class RayCube : public RayShape
{
public:
    RayCube();
    virtual ~RayCube();
    virtual glm::vec4 getNormForIntersection(glm::vec4 p, glm::vec4 d);
    virtual glm::vec2 getUV(glm::vec4 p, glm::vec4 n);

};

#endif // RAYCUBE_H
