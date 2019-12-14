#ifndef CONESHAPE_H
#define CONESHAPE_H

#include "openglshape.h"

using namespace CS123::GL;

class ConeShape : public OpenGLShape
{
public:
    ConeShape();
    ~ConeShape();

private:
    virtual void makeSides(std::vector<float>* vertices, int p1, int p2);
    void firstSide(std::vector<float>* side, int p1, int p2, float length, int dataNum);
    virtual glm::vec2 getUVfromPosition(glm::vec4 point);

};

#endif // CONESHAPE_H
