#ifndef EDGEDETECTFILTER_H
#define EDGEDETECTFILTER_H

#include "Filter.h"

class EdgeDetectFilter : public Filter
{
public:
    EdgeDetectFilter(float sensitivity);
    virtual ~EdgeDetectFilter() override;
    virtual void apply(Canvas2D *canvas, bool marquee) override;

private:
    float m_sensitivity;
    std::vector<float> m_kernel_x;
    std::vector<float> m_kernel_y;

};

#endif // EDGEDETECTFILTER_H
