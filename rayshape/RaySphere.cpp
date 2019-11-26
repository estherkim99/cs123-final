#include "RaySphere.h"

RaySphere::RaySphere()
{

}

RaySphere::~RaySphere()
{
}


glm::vec4 RaySphere::getNormForIntersection(glm::vec4 p, glm::vec4 d)
{
    glm::vec4 norm = glm::vec4(0, 0, 0, std::numeric_limits<float>::infinity());
    glm::vec4 v;

    float t1 = std::numeric_limits<float>::infinity();
    float t2 = std::numeric_limits<float>::infinity();

    float A = powf(d.x,2) + powf(d.y,2) + powf(d.z,2);
    float B = 2.f * (p.x * d.x + p.y * d.y + p.z * d.z);
    float C = powf(p.x,2) + powf(p.y,2) + powf(p.z,2) - 0.25f;
    float D = powf(B,2) - 4 * A * C;

    if (abs(A) < 0.000001f) {
        t1 = -C / B;
    }
    else if (D >= 0) {
        t1 = (-B + sqrt(D)) / (2 * A);
        t2 = (-B - sqrt(D)) / (2 * A);
    }
    v = p + t1 * d;
    if (v.y <= 0.5 && v.y >= -0.5 && t1 >= 0 && t1 < norm.w){
        norm = glm::vec4(v.x, v.y, v.z, t1);
    }
    v = p + t2 * d;
    if (v.y <= 0.5 && v.y >= -0.5 && t2 >= 0 && t2 < norm.w) {
        norm = glm::vec4(v.x, v.y, v.z, t2);
    }
    return norm;
}

glm::vec2 RaySphere::getUV(glm::vec4 p, glm::vec4 n)
{
    float x = 1.f - ((std::atan2(-n.z, -n.x) + M_PI) / (2.f * M_PI));
    float y = (std::atan2(sqrtf(powf(n.x,2.f) + powf(n.z,2.f)), n.y) / M_PI);
    return glm::vec2(x, y);
}
