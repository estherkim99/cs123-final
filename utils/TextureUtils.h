#ifndef TEXTUREUTILS_H
#define TEXTUREUTILS_H

#include "scenegraph/ListNode.h"

class TextureUtils
{
public:
    TextureUtils();

    static glm::vec3 blendTexture(glm::vec4 point, ListNode primitive);

    static glm::vec2 cubeToUV(glm::vec4 point);
    static glm::vec2 cylinderToUV(glm::vec4 point);
    static glm::vec2 sphereToUV(glm::vec4 point);
    static glm::vec3 cubeToTexture(glm::vec4 point, QImage texture, float repeatU, float repeatV);
    static glm::vec3 cylinderToTexture(glm::vec4 point, QImage texture, float repeatU, float repeatV);
    static glm::vec3 sphereToTexture(glm::vec4 point, QImage texture, float repeatU, float repeatV);

    static glm::vec2 uvToSt(glm::vec2 uv, int width, int height, int repeatU, int repeatV);

};

#endif // TEXTUREUTILS_H
