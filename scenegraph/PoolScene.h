#ifndef POOLSCENE_H
#define POOLSCENE_H
#include "OpenGLScene.h"
#include "SceneviewScene.h"

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

class PoolScene : public SceneviewScene {
public:
    PoolScene();
    virtual ~PoolScene();
    virtual void render(SupportCanvas3D *context) override;

    void init();
    void collisionDetection();
    void addVelocity(int ballNum, glm::vec3 vel);
    void tick(float secondsPassed);

protected:
    void updateTranslation(float secondsPassed);
    void renderGeometry();
    void drawObject(SceneObject o, glm::mat4 transform, int i);
    void updateVelocities(int b1, int b2);
    glm::vec3 getBallPosition(int ballnum);
    bool checkBallCollision(glm::vec3 pos1, glm::vec3 pos2);
    bool checkHoleCollision(glm::vec3 pos1, glm::vec3 pos2);
    void checkWallCollision(glm::vec3 pos, int ballnum);

    std::vector<glm::vec3> m_object_translations;
    std::vector<glm::vec3> m_object_rotations;

    std::vector<SceneObject> m_balls;
    std::vector<SceneObject> m_walls;
    std::vector<SceneObject> m_holes;

    std::vector<glm::vec3> m_ball_translations;
    std::vector<glm::vec3> m_ball_velocities;
    std::vector<glm::vec3> m_ball_rotations;
};


#endif // POOLSCENE_H
