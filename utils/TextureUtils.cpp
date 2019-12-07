#include "TextureUtils.h"

TextureUtils::TextureUtils()
{
}

// point in world space
glm::vec3 TextureUtils::blendTexture(glm::vec4 point, ListNode primitive) {
    point = glm::inverse(primitive.m_transformation) * point; // transform point to object space
    glm::vec3 texColor = glm::vec3(1, 1, 1);
    float repeatU = primitive.m_object.material.textureMap.repeatU;
    float repeatV = primitive.m_object.material.textureMap.repeatV;

    switch (primitive.m_object.type)  {
        case PrimitiveType::PRIMITIVE_CUBE: {
            texColor = cubeToTexture(point, primitive.m_texture, repeatU, repeatV);
            break;
        }
        case PrimitiveType::PRIMITIVE_CYLINDER: {
            texColor = cylinderToTexture(point, primitive.m_texture, repeatU, repeatV);
            break;
        }
        case PrimitiveType::PRIMITIVE_CONE: {
            texColor = cylinderToTexture(point, primitive.m_texture, repeatU, repeatV);
            break;
        }
        case PrimitiveType::PRIMITIVE_SPHERE: {
            texColor = sphereToTexture(point, primitive.m_texture, repeatU, repeatV);
            break;
        }
        // un-implemented shapes
        case PrimitiveType::PRIMITIVE_TORUS:
            break;
        case PrimitiveType::PRIMITIVE_MESH:
            break;
    }
    return texColor;
}

glm::vec3 TextureUtils::cubeToTexture(glm::vec4 point, QImage texture, float repeatU, float repeatV) {
    glm::vec3 color;
    glm::vec2 uv = cubeToUV(point);
    glm::vec2 st = uvToSt(uv, texture.width(), texture.height(), repeatU, repeatV);
    QRgb tex = texture.pixel(st.x, st.y);
    color = glm::vec3(qRed(tex)/255.f, qGreen(tex)/255.f, qBlue(tex)/255.f);
    return color;
}

// point in object space
glm::vec2 TextureUtils::cubeToUV(glm::vec4 point) {
    glm::vec2 uv;

    // XY plane, Z = 0.5
    if (std::abs(point.z - 0.5) <= 0.001) {
        uv = glm::vec2(point.x+0.5, 0.5-point.y);
    }

    // XY plane, Z = -0.5
    else if (std::abs(point.z + 0.5) <= 0.001) {
         uv = glm::vec2(0.5-point.x, 0.5-point.y);
    }

    // XZ plane, Y = 0.5
    else if (std::abs(point.y - 0.5) <= 0.001) {
        uv = glm::vec2(point.x+0.5, point.z+0.5);
    }

    // XZ plane, Y = -0.5
    else if (std::abs(point.y + 0.5) <= 0.001) {
        uv = glm::vec2(point.x+0.5, 0.5-point.z);
    }

    // YZ plane, X = 0.5
    else if (std::abs(point.x - 0.5) <= 0.001) {
        uv = glm::vec2(0.5-point.z, 0.5-point.y);
    }

    // YZ plane, X = -0.5
    else if (std::abs(point.x + 0.5) <= 0.001) {
        uv = glm::vec2(point.z+0.5, 0.5-point.y);
    }

    else {
        throw "Invalid point on cube";
    }

    return uv;
}

glm::vec3 TextureUtils::cylinderToTexture(glm::vec4 point, QImage texture, float repeatU, float repeatV) {
    if (std::abs(point.y + 0.5) <= 0.001 || std::abs(point.y - 0.5) <= 0.001) { // caps
        return cubeToTexture(point, texture, repeatU, repeatV);
    } else { // cone body
        glm::vec2 uv = cylinderToUV(point);
        glm::vec2 st = uvToSt(uv, texture.width(), texture.height(), repeatU, repeatV);
        QRgb tex = texture.pixel(st.x, st.y);
        return glm::vec3(qRed(tex)/255.f, qGreen(tex)/255.f, qBlue(tex)/255.f);
    }
}

glm::vec2 TextureUtils::cylinderToUV(glm::vec4 point) {
    float u;
    float theta = atan2(point.z, point.x);

    if (theta < 0) {
        u = -theta / (2*M_PI);
    } else {
        u = 1.f - (theta / (2*M_PI));
    }
    return glm::vec2(u, 0.5-point.y);
}

glm::vec3 TextureUtils::sphereToTexture(glm::vec4 point, QImage texture, float repeatU, float repeatV) {
    glm::vec2 uv = sphereToUV(point);
    glm::vec2 st = uvToSt(uv, texture.width(), texture.height(), repeatU, repeatV);
    QRgb tex = texture.pixel(st.x, st.y);
    return glm::vec3(qRed(tex)/255.f, qGreen(tex)/255.f, qBlue(tex)/255.f);
}

glm::vec2 TextureUtils::sphereToUV(glm::vec4 point) {
    float u;
    float theta = atan2(point.z, point.x);

    if (theta < 0) {
        u = -theta / (2*M_PI);
    } else {
        u = 1.f - (theta / (2*M_PI));
    }

    float v = asin(-point.y/0.5f) / M_PI + 0.5f;
    return glm::vec2(u, v);
}

glm::vec2 TextureUtils::uvToSt(glm::vec2 uv, int width, int height, int repeatU, int repeatV) {
    return glm::vec2(((int) (uv.x * repeatU * width)) % width,
                    ((int) (uv.y * repeatV * height)) % height);
}
