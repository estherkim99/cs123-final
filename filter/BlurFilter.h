#ifndef BLURFILTER_H
#define BLURFILTER_H

#include "Filter.h"
#include <vector>

class BlurFilter : public Filter
{
public:
    BlurFilter(int radius);
    virtual ~BlurFilter() override;
    virtual void apply(Canvas2D *canvas, bool marquee) override;

private:
    int m_radius;
    std::vector<float> m_kernel;

};

#endif // BLURFILTER_H
