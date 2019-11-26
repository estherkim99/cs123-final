#ifndef RAYSPHERE_H
#define RAYSPHERE_H

#include "RayShape.h"

class RaySphere : public RayShape
{
public:
    RaySphere();
    ~RaySphere();
    virtual glm::vec4 getNormForIntersection(glm::vec4 p, glm::vec4 d);
    virtual glm::vec2 getUV(glm::vec4 p, glm::vec4 n);

};

#endif // RAYSPHERE_H
