#ifndef POOLSCENE_H
#define POOLSCENE_H
#include "OpenGLScene.h"

#include <memory>

#include "shape/openglshape.h"
#include "shape/cubeshape.h"
#include "shape/cylindershape.h"
#include "shape/coneshape.h"
#include "shape/sphereshape.h"


namespace CS123 { namespace GL {

    class Shader;
    class CS123Shader;
    class Texture2D;
}}

class PoolScene : public OpenGLScene {
public:
    PoolScene();
    virtual ~PoolScene();

    virtual void render(SupportCanvas3D *context) override;
    virtual void settingsChanged() override;

    // Use this method to set an internal selection, based on the (x, y) position of the mouse
    // pointer.  This will be used during the "modeler" lab, so don't worry about it for now.
    void setSelection(int x, int y);
    void updateTranslation();
    void init();

protected:

    void loadPhongShader();
    void loadWireframeShader();
    void loadNormalsShader();
    void loadNormalsArrowShader();

    void setSceneUniforms(SupportCanvas3D *context);\
    void setMatrixUniforms(CS123::GL::Shader *shader, SupportCanvas3D *context);
    void setLights();
    void renderGeometry();

    std::unique_ptr<CS123::GL::CS123Shader> m_phongShader;
    std::unique_ptr<CS123::GL::Shader> m_wireframeShader;
    std::unique_ptr<CS123::GL::Shader> m_normalsShader;
    std::unique_ptr<CS123::GL::Shader> m_normalsArrowShader;

    void tesselateShapes();
    std::unique_ptr<CubeShape> m_cube;
    std::unique_ptr<ConeShape> m_cone;
    std::unique_ptr<CylinderShape> m_cylinder;
    std::unique_ptr<SphereShape> m_sphere;

    std::vector<glm::vec3> m_object_translations;
    std::vector<glm::vec3> m_object_rotations;
};


#endif // POOLSCENE_H
