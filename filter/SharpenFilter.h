#ifndef SHARPENFILTER_H
#define SHARPENFILTER_H

#include "Filter.h"

class SharpenFilter : public Filter
{
public:
    SharpenFilter(int radius);
    virtual ~SharpenFilter() override;
    virtual void apply(Canvas2D *canvas, bool marquee) override;

private:
    int m_radius;
    std::vector<float> m_kernel;
    void initializeKernel();
};

#endif // SHARPENFILTER_H
