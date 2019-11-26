#ifndef CUBESHAPE_H
#define CUBESHAPE_H

#include "openglshape.h"

using namespace CS123::GL;

class CubeShape : public OpenGLShape
{
public:
    CubeShape();
    ~CubeShape();

private:
    virtual void makeSides(std::vector<float>* vertices, int p1, int p2);
    void firstSide(std::vector<float>* side, int p1, float length, int dataNum);

};

#endif // CUBESHAPE_H
