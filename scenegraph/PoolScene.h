#ifndef POOLSCENE_H
#define POOLSCENE_H
#include "OpenGLScene.h"

#include <memory>

#include "shape/openglshape.h"
#include "shape/cubeshape.h"
#include "shape/cylindershape.h"
#include "shape/coneshape.h"
#include "shape/sphereshape.h"

#include <glm.hpp>
#include "CS123SceneData.h"
#include <list>
#include "SupportCanvas2D.h"

namespace CS123
{
namespace GL
{

namespace CS123
{
namespace GL
{

class Shader;
class CS123Shader;
class Texture2D;
} // namespace GL
} // namespace CS123

class PoolScene : public OpenGLScene
{
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

    void setSceneUniforms(SupportCanvas3D *context);
    void setMatrixUniforms(CS123::GL::Shader *shader, SupportCanvas3D *context);
    void setLights();
    void renderGeometry();
    void drawObject(SceneObject o, glm::mat4 transform, int i);
    void updateVelocities(int b1, int b2);
    //    glm::vec3 getBallPosition(int ballnum);
    bool checkBallCollision(glm::vec3 pos1, glm::vec3 pos2);
    bool checkHoleCollision(glm::vec3 pos1, glm::vec3 pos2);
    void checkWallCollision(glm::vec3 pos, int ballnum);

    std::unique_ptr<CS123::GL::CS123Shader> m_phongShader;
    std::unique_ptr<CS123::GL::Shader> m_wireframeShader;
    std::unique_ptr<CS123::GL::Shader> m_normalsShader;
    std::unique_ptr<CS123::GL::Shader> m_normalsArrowShader;

    void tesselateShapes();
    std::unique_ptr<CubeShape> m_cube;
    std::unique_ptr<ConeShape> m_cone;
    std::unique_ptr<CylinderShape> m_cylinder;
    std::unique_ptr<SphereShape> m_sphere;

    std::vector<glm::vec3> m_ball_translations;
    std::vector<glm::vec3> m_ball_velocities;
    std::vector<glm::vec3> m_ball_rotations;
    std::vector<bool> m_ball_done;
};

#endif // POOLSCENE_H
