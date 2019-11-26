#ifndef FILTER_H
#define FILTER_H

#include <assert.h>
#include "RGBA.h"
#include "Canvas2D.h"
#include "RGBA.h"

class Canvas2D;

class Filter
{
public:
    virtual ~Filter();

    virtual void apply(Canvas2D *canvas, bool marquee) = 0;

protected:
    void updateMarquee(Canvas2D *canvas, std::vector<RGBA> *result);
};

#endif // FILTER_H
