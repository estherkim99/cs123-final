#include "RayCylinder.h"

RayCylinder::RayCylinder()
{
}

RayCylinder::~RayCylinder()
{
}

glm::vec4 RayCylinder::getNormForIntersection(glm::vec4 p, glm::vec4 d)
{
    glm::vec4 norm = glm::vec4(0, 0, 0, std::numeric_limits<float>::infinity());
    glm::vec4 v;

    // cylinder body
    float t1 = std::numeric_limits<float>::infinity();
    float t2 = std::numeric_limits<float>::infinity();
    float A = powf(d.x,2) + powf(d.z,2);
    float B = 2 * (p.x * d.x + p.z * d.z);
    float C = powf(p.x,2) + powf(p.z,2) - 0.25;
    float D = powf(B,2) - 4 * A * C;

    if (abs(A) < 0.000001f) {
        t1 = -C / B;
    }
    else if (D >= 0) {
        t1 = (-B + sqrtf(D)) / (2 * A);
        t2 = (-B - sqrtf(D)) / (2 * A);
    }
    // check if t is in boundary, update norm if intersect
    v = p + t1 * d;
    if (v.y <= 0.5 && v.y >= -0.5 && t1 >= 0 && t1 < norm.w){
        norm = glm::vec4(v.x, 0, v.z, t1);
    }
    v = p + t2 * d;
    if (v.y <= 0.5 && v.y >= -0.5 && t2 >= 0 && t2 < norm.w) {
        norm = glm::vec4(v.x, 0, v.z, t2);
    }

    // cylinder top and bottom caps
    float t3 = (0.5f - p.y) / d.y; // top cap
    float t4 = (-0.5f - p.y) / d.y; // bottom cap
    v = p + t3 * d;
    if (v.x * v.x + v.z * v.z <= 0.25f && t3 >= 0 && t3 < norm.w) {
        norm = glm::vec4(0, 1, 0, t3);
    }
    v = p + t4 * d;
    if (v.x * v.x + v.z * v.z <= 0.25f && t4 >= 0.f && t4 < norm.w) {
        norm = glm::vec4(0, -1, 0, t4);
    }

    return norm;
}


glm::vec2 RayCylinder::getUV(glm::vec4 p, glm::vec4 n)
{
    // top cap
    if (abs(n.y - 1.f) < 0.000001f) {
        return glm::vec2(0.5f + p.x, 0.5f + p.z);
    }

    // bottom cap
    if (abs(n.y + 1.f) < 0.000001f) {
        return glm::vec2(0.5f + p.x, 0.5f - p.z);
    }

    // cylinder body
    float u = (std::atan2(-p.z, -p.x) + M_PI) / (2.f * M_PI);
    return glm::vec2(1.f - u, 0.5f - p.y);
}
