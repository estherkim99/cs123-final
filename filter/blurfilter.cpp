#include "BlurFilter.h"
#include "FilterUtils.h"
#include <cmath>

BlurFilter::BlurFilter(int radius) :
    m_radius(radius)
{
    FilterUtils::gaussianKernel(&m_kernel, radius);
}

BlurFilter::~BlurFilter()
{
}

void BlurFilter::apply(Canvas2D *canvas, bool marquee) {
    std::vector<RGBA> result;
    FilterUtils::Convolve1D(canvas->data(), canvas->width(), canvas->height(), &result, m_kernel, FilterUtils::DIR_VERT);
    if (marquee) {
        Filter::updateMarquee(canvas, &result);
    } else {
        memcpy(canvas->data(), result.data(), sizeof(RGBA) * result.size());
    }
    FilterUtils::Convolve1D(canvas->data(), canvas->width(), canvas->height(), &result, m_kernel, FilterUtils::DIR_HORZ);
    if (marquee) {
        Filter::updateMarquee(canvas, &result);
    } else {
        memcpy(canvas->data(), result.data(), sizeof(RGBA) * result.size());
    }
}
