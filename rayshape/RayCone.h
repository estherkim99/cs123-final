#ifndef RAYCONE_H
#define RAYCONE_H

#include "RayShape.h"

class RayCone : public RayShape
{
public:
    RayCone();
    virtual ~RayCone();
    virtual glm::vec4 getNormForIntersection(glm::vec4 p, glm::vec4 d);
    virtual glm::vec2 getUV(glm::vec4 p, glm::vec4 n);

};

#endif // RAYCONE_H
