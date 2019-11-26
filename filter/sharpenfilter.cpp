#include "SharpenFilter.h"
#include "FilterUtils.h"

SharpenFilter::SharpenFilter(int radius) :
    m_radius(radius)
{
initializeKernel();
}

SharpenFilter::~SharpenFilter() {

}


void SharpenFilter::initializeKernel() {
    FilterUtils::gaussianKernel(&m_kernel, m_radius);
    for (int i = 0; i < m_kernel.size(); i++) {
        m_kernel.at(i) *= -1.f;
    }
    m_kernel.at(m_radius) += 2.f;

}

void SharpenFilter::apply(Canvas2D *canvas, bool marquee){
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
