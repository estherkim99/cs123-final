/**
 * @file   LinearBrush.cpp
 *
 * Implementation of a brush with a linear mask distribution.
 *
 * You should fill this file in while completing the Brush assignment.
 */

#include "LinearBrush.h"

LinearBrush::LinearBrush(RGBA color, int radius)
    : Brush(color, radius)
{
    // @TODO: [BRUSH] You'll probably want to set up the mask right away.
    makeMask();
}


LinearBrush::~LinearBrush()
{
}

void LinearBrush::makeMask() {
    m_mask.reserve(m_radius + 1);
    for (int i = 0; i < m_radius + 1; i++) {
       m_mask[i] = 1 - (static_cast<float>(i) / m_radius);
    }
}
