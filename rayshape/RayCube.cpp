#include "RayCube.h"

RayCube::RayCube()
{
}

RayCube::~RayCube(){

}

glm::vec4 RayCube::getNormForIntersection(glm::vec4 p, glm::vec4 d) {
    glm::vec4 norm = glm::vec4(0, 0, 0, std::numeric_limits<float>::infinity());
    glm::vec4 noIntersect = norm;
    float tmin, tmax, tymin, tymax, txmin, txmax;

    tmin = (((d.z < 0) ? 0.5f : -0.5f) - p.z) / d.z;
    tmax = (((d.z < 0) ? -0.5f : 0.5f) - p.z) / d.z;
    tymin = (((d.y < 0) ? 0.5f : -0.5f) - p.y) / d.y;
    tymax = (((d.y < 0) ? -0.5f : 0.5f) - p.y) / d.y;
    if (tmin > tymax || tymin > tmax) {
        return noIntersect;
    }

    if (tymin > tmin) {
        tmin = tymin;
        norm = glm::vec4(0, ((d.y < 0) ? 1 : -1), 0, tmin);
    } else {
        norm = glm::vec4(0, 0, ((d.z < 0) ? 1 : -1), tmin);
    }
    if (tymax < tmax) {
        tmax = tymax;
    }

    txmin = (((d.x < 0) ? 0.5f : -0.5f) - p.x) / d.x;
    txmax = (((d.x < 0) ? -0.5f : 0.5f) - p.x) / d.x;

    if (tmin > txmax || txmin > tmax){
        return noIntersect;
    }

    if (txmin > tmin) {
        tmin = txmin;
        norm = glm::vec4(((d.x < 0) ? 1 : -1), 0, 0, tmin);
    }

    if (txmax < tmax){
        tmax = txmax;
    }
    if (tmin > 0 && tmax < std::numeric_limits<float>::infinity()) {
        return norm;
    }
    else {
        return noIntersect;
    }
 }


glm::vec2 RayCube::getUV(glm::vec4 p, glm::vec4 n)
{
    glm::vec2 ret = glm::vec2(-1.f);

    if (abs(n.x - 1.f) < 0.000001f) {
        ret = glm::vec2(0.5f - p.z, 0.5f - p.y);
    }
    if (abs(n.x + 1.f) < 0.000001f) {
        ret = glm::vec2(0.5f + p.z, 0.5f - p.y);
    }
    if (abs(n.y - 1.f) < 0.000001f) {
        ret = glm::vec2(0.5f + p.x, 0.5f + p.z);
    }
    if (abs(n.y + 1.f) < 0.000001f) {
        ret = glm::vec2(0.5f + p.x, 0.5f - p.z);
    }
    if (abs(n.z - 1.f) < 0.000001f) {
        ret = glm::vec2(0.5f + p.x, 0.5f - p.y);
    }
    if (abs(n.z + 1.f) < 0.000001f) {
        ret = glm::vec2(0.5f - p.x, 0.5f - p.y);
    }
    return ret;
}
