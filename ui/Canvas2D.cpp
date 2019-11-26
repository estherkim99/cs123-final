/**
 * @file Canvas2D.cpp
 *
 * CS123 2-dimensional canvas. Contains support code necessary for Brush, Filter, Intersect, and
 * Ray.
 *
 * YOU WILL NEED TO FILL THIS IN!
 *
 */

// For your convenience, a few headers are included for you.
#include <assert.h>
#include <iostream>
#include <math.h>
#include <memory>
#include <unistd.h>
#include "Canvas2D.h"
#include "Settings.h"
#include "RayScene.h"
#include "SceneviewScene.h"

#include <QPainter>
#include "brush/Brush.h"
#include "brush/ConstantBrush.h"
#include "brush/LinearBrush.h"
#include "brush/QuadraticBrush.h"
#include "brush/SmudgeBrush.h"

#include "filter/Filter.h"
#include "filter/BlurFilter.h"
#include "filter/EdgeDetectFilter.h"
#include "filter/ScaleFilter.h"
#include "filter/SharpenFilter.h"

Canvas2D::Canvas2D() :
    // @TODO: Initialize any pointers in this class here.
    m_filter(nullptr),
    m_rayScene(nullptr)
{
}

Canvas2D::~Canvas2D()
{
}

// This is called when the canvas size is changed. You can change the canvas size by calling
// resize(...). You probably won't need to fill this in, but you can if you want to.
void Canvas2D::notifySizeChanged(int w, int h) {
}

void Canvas2D::paintEvent(QPaintEvent *e) {
    // You probably won't need to fill this in, but you can if you want to override any painting
    // events for the 2D canvas. For now, we simply call the superclass.
    SupportCanvas2D::paintEvent(e);

}

void Canvas2D::settingsChanged() {
    // TODO: Process changes to the application settings.
    newBrush();


}

// ********************************************************************************************
// ** BRUSH
// ********************************************************************************************

void Canvas2D::newBrush() {
    RGBA color = settings.brushColor;
    int radius = settings.brushRadius;

    switch (settings.brushType) {
    case BRUSH_CONSTANT :
        m_brush = std::make_unique<ConstantBrush>(color, radius);
        break;
    case BRUSH_LINEAR :
        m_brush = std::make_unique<LinearBrush>(color, radius);
        break;
    case BRUSH_QUADRATIC :
        m_brush = std::make_unique<QuadraticBrush>(color, radius);
        break;
    case BRUSH_SMUDGE :
        m_brush = std::make_unique<SmudgeBrush>(color, radius);
        break;
    }
}

void Canvas2D::mouseDown(int x, int y) {
    // @TODO: [BRUSH] Mouse interaction for brush. You will probably want to create a separate
    //        class for each of your brushes. Remember that you can use the static Settings
    //        object to get the currently selected brush and its parameters.

    // You're going to need to leave the alpha value on the canvas itself at 255, but you will
    // need to use the actual alpha value to compute the new color of the pixel
    m_brush->brushDown(x, y, this);

    //    bool fixAlphaBlending = settings.fixAlphaBlending; // for extra/half credit
}

void Canvas2D::mouseDragged(int x, int y) {
    // TODO: [BRUSH] Mouse interaction for Brush.
    m_brush->brushDragged(x, y, this);
}

void Canvas2D::mouseUp(int x, int y) {
    // TODO: [BRUSH] Mouse interaction for Brush.
    m_brush->brushUp(x, y, this);
}



// ********************************************************************************************
// ** FILTER
// ********************************************************************************************

void Canvas2D::filterImage() {

    // TODO: [FILTER] Filter the image. Some example code to get the filter type is provided below.
    switch(settings.filterType) {
    case FILTER_BLUR:
        m_filter = std::make_unique<BlurFilter>(settings.blurRadius);
        break;
    case FILTER_EDGE_DETECT:
        m_filter = std::make_unique<EdgeDetectFilter>(settings.edgeDetectSensitivity);
        break;
    case FILTER_SCALE:
        m_filter = std::make_unique<ScaleFilter>(settings.scaleX, settings.scaleY);
        break;
    case FILTER_SHARPEN:
        m_filter = std::make_unique<SharpenFilter>(settings.sharpenRadius);
        break;
    }

    if (m_filter) {
    if (marqueeStart()==marqueeStop()) {
        m_filter->apply(this, false);
    } else {
        m_filter->apply(this, true);
    }
    this->update();
    }
}

// ********************************************************************************************
// ** RAY
// ********************************************************************************************

void Canvas2D::setScene(RayScene *scene) {
    m_rayScene.reset(scene);
}

void Canvas2D::renderImage(Camera *camera, int width, int height) {
    if (m_rayScene) {

        // @TODO: raytrace the scene based on settings
        //        YOU MUST FILL THIS IN FOR INTERSECT/RAY
        // If you want the interface to stay responsive, make sure to call
        // QCoreApplication::processEvents() periodically during the rendering.
        this->resize(width, height);
        m_rayScene->rayTrace(camera, this);


        this->update();


    }

}

void Canvas2D::cancelRender() {
    // TODO: cancel the raytracer (optional)
}




