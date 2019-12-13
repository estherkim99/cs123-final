#ifndef SPHERESHAPE_H
#define SPHERESHAPE_H

#include "openglshape.h"

using namespace CS123::GL;

class SphereShape : public OpenGLShape
{
public:
    SphereShape();
    ~SphereShape();

private:
    virtual void makeSides(std::vector<float>* vertices, int p1, int p2);
    void firstSide(std::vector<float>* side, int p1, int p2, float length, int dataNum);
    virtual glm::vec2 getUVfromPosition(glm::vec4 point);
};

#endif // SPHERESHAPE_H
