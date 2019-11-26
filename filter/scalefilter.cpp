#include "ScaleFilter.h"

#include <cmath>
#include "RGBA.h"

ScaleFilter::ScaleFilter(float scaleX, float scaleY) :
    m_scaleX(scaleX),
    m_scaleY(scaleY)
{

}

ScaleFilter::~ScaleFilter() {

}


void ScaleFilter::apply(Canvas2D *canvas, bool marquee){
    int neww = floor(canvas->width() * m_scaleX);
    int newh = canvas->height();

    std::vector<RGBA> result;
    result.resize(neww * newh);

    for (int r = 0; r < neww; r++) {
        for (int c = 0; c < newh; c++) {
            float rsum = 0.f;
            float gsum = 0.f;
            float bsum = 0.f;
            float weights_sum = 0.f;
            float center = c/m_scaleX +  (1-m_scaleX)/(2*m_scaleX);
            float support = (m_scaleX  > 1) ? 1 : 1.f /m_scaleX;
            int left = ceil(center - support);
            int right = floor(center + support);

            for (int i = left; i <=  right; i++) {
                if (i < 0 || i > canvas->width()) {
                    continue;
                }
                rsum += g(i - center,  m_scaleX) * static_cast<float>(canvas->data()[r * canvas->width() + i].r) / 255.f;
                gsum += g(i - center,  m_scaleX) * static_cast<float>(canvas->data()[r * canvas->width() + i].g) / 255.f;
                bsum += g(i - center,  m_scaleX) * static_cast<float>(canvas->data()[r * canvas->width() + i].b) / 255.f;
                weights_sum += g(i - center, m_scaleX);
            }
            rsum = rsum/weights_sum * 255;
            gsum = gsum/weights_sum * 255;
            bsum = bsum/weights_sum * 255;
            RGBA newRGBA = RGBA(rsum, gsum, bsum, 255);
            result.at(r * neww + c) = newRGBA;
        }
    }


    canvas->resize(neww, newh);
    if (marquee) {
        Filter::updateMarquee(canvas, &result);
    } else {
        memcpy(canvas->data(), result.data(), sizeof(RGBA) * result.size());
    }

}

float ScaleFilter::g(float x, float a) {
    float radius;
    if (a < 1) {
        radius = 1.0/a;
    } else {
        radius = 1.0;
    }
    if ((x < -radius) || x > radius) {
        return 0;
    } else {
        return (1 - fabs(x)/radius) / radius;
    }
}
