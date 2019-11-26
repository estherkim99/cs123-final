#include "FilterUtils.h"
#include "math.h"
#include <vector>

namespace FilterUtils
{

inline unsigned char REAL2byte(float f) {
    int i = static_cast<int>((f * 255.0 + 0.5));
    return (i < 0) ? 0 : (i > 255) ? 255 : i;
}

float gaussian(int x, int mu, float sdev) {
    return expf(-0.5f * powf((x-mu)/sdev, 2.f)) / sqrtf(2.f * M_PI * sdev * sdev);;
}

void gaussianKernel(std::vector<float> *kernel, int radius) {
    kernel->resize(radius * 2 + 1);
    float sdev = radius / 3.f;
    float kernelSize = 0;
    for (int i = 0; i < radius * 2 + 1; i++) {
        kernel->at(i) = gaussian(i, radius, sdev);
        kernelSize += kernel->at(i);
    }
}



void Convolve2D(RGBA* data, int width, int height, std::vector<RGBA> *result, const std::vector<float> &kernel) {
    result->resize(width * height);
    int dimension = static_cast<int>(sqrt(kernel.size()));

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            size_t centerIndex = r * width + c;
            float red_acc = 0.f;
            float green_acc = 0.f;
            float blue_acc = 0.f;
            float correctBrightness = 0;
            for (int h = 0; h < dimension; h++) {
                for (int w = 0; w < dimension; w++) {
                    float kern_val = kernel.at(h * dimension + w);
                    int image_r = r - ((dimension - 1)/2) + h;
                    int image_c = c - ((dimension - 1)/2) + w;
                    if (image_r < 0 || image_r >= height || image_c < 0 || image_c >= width) {
                        correctBrightness += kern_val;
                        continue;
                    }
                    int image_index = image_r * width + image_c;
                    red_acc = red_acc + kern_val * static_cast<float>(data[image_index].r / 255.f) ;
                    green_acc = green_acc + kern_val * static_cast<float>(data[image_index].g / 255.f);
                    blue_acc = blue_acc + kern_val * static_cast<float>(data[image_index].b / 255.f);
                }
            }
            red_acc /= (1 - correctBrightness);
            green_acc /= (1 - correctBrightness);
            blue_acc /= (1 - correctBrightness);
            RGBA newRGBA = RGBA(REAL2byte(red_acc), REAL2byte(green_acc), REAL2byte(blue_acc), 255);
            result->at(centerIndex) = newRGBA;

        }
    }

}

void Convolve1D(RGBA* data, int width, int height,std::vector<RGBA> *result, const std::vector<float> &kernel, dir direction) {
    result->resize(width * height);
    int dimension = kernel.size();

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            size_t centerIndex = r * width + c;
            float red_acc = 0.f;
            float green_acc = 0.f;
            float blue_acc = 0.f;
            float correctBrightness = 0.f;
            for (int k = 0; k < dimension; k++) {
                int imageLoc;
                int imageLen;
                int imagePixel;
                if (direction == DIR_HORZ) {
                    imageLoc = c - ((dimension - 1)/2) + k;
                    imageLen = width;
                    imagePixel = r * width + imageLoc;

                } else {
                    imageLoc = r - ((dimension - 1)/2) + k;
                    imageLen = height;
                    imagePixel = imageLoc * width + c;
                }
                if (imageLoc < 0 || imageLoc >= imageLen) {
                    continue;
                }
                correctBrightness += kernel.at(k);
                red_acc += kernel.at(k) * static_cast<float>(data[imagePixel].r / 255.f) ;
                green_acc += kernel.at(k) * static_cast<float>(data[imagePixel].g / 255.f);
                blue_acc += kernel.at(k) * static_cast<float>(data[imagePixel].b / 255.f);
            }
            red_acc /= (correctBrightness);
            green_acc /= (correctBrightness);
            blue_acc /= (correctBrightness);
            RGBA newRGBA = RGBA(REAL2byte(red_acc), REAL2byte(green_acc), REAL2byte(blue_acc), 255);
            result->at(centerIndex) = newRGBA;
        }
    }
}



}
