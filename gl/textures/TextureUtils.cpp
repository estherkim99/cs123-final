//#include "TextureUtils.h"

//TextureUtils::TextureUtils()
//{

//}

//// should be better off in Scene?
//// point in world space
//// glm::vec3 TextureUtils::blendTexture(glm::vec4 point, Scene::SceneObject o) {
////     point = glm::inverse(o.composite) * point; // transform point to object space
////     glm::vec3 texColor = glm::vec3(1, 1, 1);
////     float repeatU = o.material.textureMap.repeatU;
////     float repeatV = o.material.textureMap.repeatV;

////     int objID = o.id;

////     switch (primitive.m_object.type)  {
////         case PrimitiveType::PRIMITIVE_CUBE: {
////             texColor = cubeToTexture(point, primitive.texture, repeatU, repeatV);
////             break;
////         }
////         case PrimitiveType::PRIMITIVE_CYLINDER: {
////             texColor = cylinderToTexture(point, primitive.texture, repeatU, repeatV);
////             break;
////         }
////         case PrimitiveType::PRIMITIVE_CONE: {
////             texColor = cylinderToTexture(point, primitive.texture, repeatU, repeatV);
////             break;
////         }
////         case PrimitiveType::PRIMITIVE_SPHERE: {
////             texColor = sphereToTexture(point, primitive.texture, repeatU, repeatV);
////             break;
////         }
////         // un-implemented shapes
////         case PrimitiveType::PRIMITIVE_TORUS:
////             break;
////         case PrimitiveType::PRIMITIVE_MESH:
////             break;
////     }
////     return texColor;
//// }

// glm::vec3 TextureUtils::cubeToTexture(glm::vec4 point, QImage texture, float repeatU, float repeatV) {
//     glm::vec3 color;
//     glm::vec2 uv = cubeToUV(point);
//     glm::vec2 st = uvToSt(uv, texture.width(), texture.height(), repeatU, repeatV);
//     QRgb tex = texture.pixel(st.x, st.y);
//     color = glm::vec3(qRed(tex)/255.f, qGreen(tex)/255.f, qBlue(tex)/255.f);
//     return color;
// }


// glm::vec3 TextureUtils::cylinderToTexture(glm::vec4 point, QImage texture, float repeatU, float repeatV) {
//     if (std::abs(point.y + 0.5) <= 0.001 || std::abs(point.y - 0.5) <= 0.001) { // caps
//         return cubeToTexture(point, texture, repeatU, repeatV);
//     } else { // cone body
//         glm::vec2 uv = cylinderToUV(point);
//         glm::vec2 st = uvToSt(uv, texture.width(), texture.height(), repeatU, repeatV);
//         QRgb tex = texture.pixel(st.x, st.y);
//         return glm::vec3(qRed(tex)/255.f, qGreen(tex)/255.f, qBlue(tex)/255.f);
//     }
// }

// glm::vec3 TextureUtils::sphereToTexture(glm::vec4 point, QImage texture, float repeatU, float repeatV) {
//     glm::vec2 uv = sphereToUV(point);
//     glm::vec2 st = uvToSt(uv, texture.width(), texture.height(), repeatU, repeatV);
//     QRgb tex = texture.pixel(st.x, st.y);
//     return glm::vec3(qRed(tex)/255.f, qGreen(tex)/255.f, qBlue(tex)/255.f);
// }


// glm::vec2 TextureUtils::uvToSt(glm::vec2 uv, int width, int height, int repeatU, int repeatV) {
//     return glm::vec2(((int) (uv.x * repeatU * width)) % width,
//                     ((int) (uv.y * repeatV * height)) % height);
//}
