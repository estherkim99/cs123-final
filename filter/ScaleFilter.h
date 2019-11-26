#ifndef SCALEFILTER_H
#define SCALEFILTER_H

#include "Filter.h"

class ScaleFilter : public Filter
{
public:
    ScaleFilter(float scaleX, float scakeY);
    virtual ~ScaleFilter() override;
    virtual void apply(Canvas2D *canvas, bool marquee) override;

private:
    float m_scaleX;
    float m_scaleY;
    float g(float x, float a);
};

#endif // SCALEFILTER_H
