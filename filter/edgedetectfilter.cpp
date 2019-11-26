#include "EdgeDetectFilter.h"
#include "FilterUtils.h"

#include <cmath>
#include <iostream>

unsigned char RGBAToGray(const RGBA &pixel) {
    unsigned char r = pixel.r;
    unsigned char g = pixel.g;
    unsigned char b = pixel.b;
    unsigned char luminosity = static_cast<unsigned char>(0.299 * r + 0.587 * g + 0.114 * b);
    return luminosity;
}

EdgeDetectFilter::EdgeDetectFilter(float sensitivity) :
    m_sensitivity(sensitivity),
    m_kernel_x({-1.f, 0.f, 1.f, -2.f, 0.f, 2.f, -1.f, 0.f, 1.f}),
    m_kernel_y({-1.f, -2.f, -1.f, 0.f, 0.f, 0.f, 1.f, 2.f, 1.f})
{

}

EdgeDetectFilter::~EdgeDetectFilter() {

}

void EdgeDetectFilter::apply(Canvas2D *canvas, bool marquee){
    int height = canvas->height();
    int width = canvas->width();
    std::vector<RGBA> result;
    result.resize(height * width);
    RGBA* data = canvas->data();

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            float g;
            if (r == 0 || c == 0 || r == height - 1 || c == width - 1) {
                g = 0;
            } else {
               float accX = 0;
               float accY = 0;
               for (int h = 0; h < 3; h++) {
                    for (int w = 0; w < 3; w++){
                        int image_r = r + h - 1;
                        int image_c = c + w - 1;
                        accX += m_kernel_x[h * 3 + w] * (RGBAToGray(data[image_r * width + image_c]) / 255.f);
                        accY += m_kernel_y[h * 3 + w] * (RGBAToGray(data[image_r * width + image_c]) / 255.f);
                    }
                }
                g = sqrtf(powf(accX,2) + powf(accY,2));
                g = g * m_sensitivity;
                if (g < 0) {
                    g = 0;
                } else if (g > 1) {
                    g = 1;
                } else {
                    g = g;
                }
                g = g * 255;
            }

            RGBA newRGBA = RGBA(g, g, g, 255);
            result.at(r * width + c) = newRGBA;
        }
    }

    if (marquee) {
        Filter::updateMarquee(canvas, &result);
    } else {
        memcpy(canvas->data(), result.data(), sizeof(RGBA) * result.size());
    }

}
