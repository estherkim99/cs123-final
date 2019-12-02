#ifndef SHAPE_H
#define SHAPE_H

/**
 * @class Shape
 *
 * @brief Parent class for shapes in the scenegraph.
 */
class Shape {

public:
    Shape();
    virtual ~Shape();

    virtual void createVertices(int p1, int p2) = 0;
    virtual void drawShape() = 0;

};

#endif // SHAPE_H
