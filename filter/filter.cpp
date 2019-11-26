#include "Filter.h"
#include <cmath>


Filter::~Filter()
{
}


void Filter::updateMarquee(Canvas2D *canvas, std::vector<RGBA> *result){
    int l = fmin(canvas->marqueeStart().x(), canvas->marqueeStop().x());
    int r = fmax(canvas->marqueeStart().x(), canvas->marqueeStop().x());
    int b = fmin(canvas->marqueeStart().y(), canvas->marqueeStop().y());
    int t = fmax(canvas->marqueeStart().y(), canvas->marqueeStop().y());
    for (int h = 0; h < canvas->height(); h++) {
        for (int w = 0; w < canvas->width(); w++) {
            if (w > l && w < r && h > b && h < t) {
                canvas->data()[h * canvas->width() + w] = result->at(h * canvas->width() + w);
            }
            canvas->data()[h * canvas->width() + w] = canvas->data()[h * canvas->width() + w];
        }

    }
}
