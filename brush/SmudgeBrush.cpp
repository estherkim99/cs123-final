/**
 * @file   Brush.cpp
 *
 * Implementation of a brush that smudges the image on the canvas as the mouse is dragged.
 *
 * You should fill this file in for the Brush assignment.
 */

#include "SmudgeBrush.h"
#include "Canvas2D.h"

#include <cmath>
#include <iostream>

static unsigned char lerp(unsigned char a, unsigned char b, float percent) {
    float fa = static_cast<float>(a) / 255.0f;
    float fb = static_cast<float>(b) / 255.0f;
    return static_cast<unsigned char>((fa + (fb - fa) * percent) * 255.0f + 0.5f);
}

SmudgeBrush::SmudgeBrush(RGBA color, int radius) :
    QuadraticBrush(color, radius)
{
    // @TODO: [BRUSH] Initialize any memory you are going to use here. Hint - you are going to
    //       need to store temporary image data in memory. Remember to use automatically managed memory!
    makeMask();
}


SmudgeBrush::~SmudgeBrush()
{
    // @TODO: [BRUSH] Be sure not to leak memory!  If you're using automatic memory management everywhere,
    //       this will be empty.
}

void SmudgeBrush::brushUp(int x, int y, Canvas2D* canvas) {
}


//! create a mask with a distribution of your choice (probably want to use quadratic for best results)
void SmudgeBrush::makeMask() {
    // @TODO: [BRUSH] Set up the mask for your brush here. For this brush you will probably want
    //        to use a quadratic distribution for the best results. Linear or Gaussian would
    //        work too, however. Feel free to paste your code from the Linear or Quadratic brushes
    //        or modify the class inheritance to be able to take advantage of one of those class's
    //        existing implementations. The choice is yours!
    //

    QuadraticBrush::makeMask();
}

void SmudgeBrush::brushDown(int x, int y, Canvas2D *canvas) {
    pickUpPaint(x, y, canvas);
}

//! Picks up paint from the canvas before drawing begins.
void SmudgeBrush::pickUpPaint(int x, int y, Canvas2D* canvas) {
    m_lastX = x;
    m_lastY = y;
    m_temp = canvas->data();
}

void SmudgeBrush::brushDragged(int mouseX, int mouseY, Canvas2D* canvas) {
    // @TODO: [BRUSH] Here, you'll need to paste down the paint you picked up in
    //        the previous method. Be sure to take the mask into account! You can
    //        ignore the alpha parameter, but you can also use it (smartly) if you
    //        would like to.

    RGBA *pix = canvas->data();
    int r = m_radius;
    int rowstart = std::max(0, mouseY - r);
    int rowend = std::min(canvas->height(), mouseY + r + 1);
    int colstart = std::max(0, mouseX - r);
    int colend = std::min(canvas->width(), mouseX + r + 1);
    int rowcounter, colcounter;

        for (rowcounter = rowstart; rowcounter < rowend; rowcounter++) {
            for (colcounter = colstart; colcounter < colend; colcounter++) {
                int dst = (int) (round(sqrt(pow((mouseY - rowcounter), 2) + pow((mouseX - colcounter), 2))));
                if (dst < r + 1) {
                    float m = m_mask[dst];
                    int canvas_index = rowcounter * canvas->width() + colcounter;
                    int copyFromX = m_lastX + (colcounter - mouseX);
                    int copyFromY = m_lastY + (rowcounter - mouseY);
                    int newIndex = copyFromY * canvas->width() + copyFromX;
                    RGBA temp = pix[canvas_index];
                    if (copyFromY > 0 && copyFromX > 0 && copyFromY < canvas->height() && copyFromX < canvas->width()) {
                    RGBA brushColor = m_temp[newIndex];
                    unsigned char newR = lerp(temp.r, brushColor.r, m * getAlpha() / 256);
                    unsigned char newG = lerp(temp.g, brushColor.g, m * getAlpha() / 256);
                    unsigned char newB = lerp(temp.b, brushColor.b, m * getAlpha() / 256);
                    pix[canvas_index] = RGBA(newR, newG, newB, getAlpha());
                    }
                }
            }
        }
        canvas->update();

            // now pick up paint again...
    pickUpPaint(mouseX, mouseY, canvas);

}
