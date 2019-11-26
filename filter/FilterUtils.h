#ifndef FILTERUTILS_H
#define FILTERUTILS_H

#include "RGBA.h"
#include <vector>

namespace FilterUtils
{
enum dir {DIR_HORZ, DIR_VERT};
inline unsigned char REAL2byte(float f);
float gaussian(int x, int mu, float sdev);
void gaussianKernel(std::vector<float> *kernel, int radius);
void Convolve2D(RGBA* data, int width, int height, std::vector<RGBA> *result, const std::vector<float> &kernel);
void Convolve1D(RGBA* data, int width, int height, std::vector<RGBA> *result, const std::vector<float> &kernel, dir direction);

}

#endif // FILTERUTILS_H
