I made four classes that inherits super class Filter, for Blur, EdgeDetectFilter, ScaleFilter and SharpenFilter (special 1).

Filter is an abstract class that defines an abstract function apply and protected function updateMarquee. UpdateMarquee is used to implement partial filtering of specified area using a marquee tool.

The special filter I chose to implement is a sharpen filter. I used a gaussian kernel to define negative weights for pixels around the center pixel, and added 2.f weights on that center pixel, in order to have a sharpening effect.

Since gaussian kernels are used in both blur and sharpen, I defined them in namespace FilterUtils, along with convolve2D and convolve1D, which are used by the filters to do convolution.

Bugs or errors -> I failed to remove error for scale filter - program fails with vector out of range.