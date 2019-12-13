#include "PoolScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"

#include <set> // for set operations

#include <utility>
#include <gl/textures/Texture2D.h>
#include "gl/textures/TextureParameters.h"
#include "gl/textures/TextureParametersBuilder.h"

using namespace CS123::GL;

PoolScene::PoolScene() : SceneviewScene()
{

    init();
}

PoolScene::~PoolScene()
{
}

void PoolScene::init()
{
    m_object_rotations.clear();
    m_object_translations.clear();
    for (int i = 0; i < m_sceneObjects.size(); i++)
    {
        m_object_translations.push_back(glm::vec3(0.0f));
        m_object_rotations.push_back(glm::vec3(0.0f));
    }
}

void PoolScene::loadPhongShader()
{
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/default.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void PoolScene::loadWireframeShader()
{
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.frag");
    m_wireframeShader = std::make_unique<Shader>(vertexSource, fragmentSource);
}

void PoolScene::loadNormalsShader()
{
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normals.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.frag");
    m_normalsShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void PoolScene::loadNormalsArrowShader()
{
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.frag");
    m_normalsArrowShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void PoolScene::render(SupportCanvas3D *context)
{
    setClearColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_mustLoadTextures)
        loadTextures();

    m_phongShader->bind();
    setSceneUniforms(context);
    setLights();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    renderGeometry();
    glBindTexture(GL_TEXTURE_2D, 0);
    m_phongShader->unbind();

    if (settings.drawWireframe)
    {
        m_wireframeShader->bind();
        setMatrixUniforms(m_wireframeShader.get(), context);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        renderGeometry();
        m_wireframeShader->unbind();
    }

    if (settings.drawNormals)
    {
        // Render the lines.
        m_normalsShader->bind();
        setMatrixUniforms(m_normalsShader.get(), context);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        renderGeometry();
        m_normalsShader->unbind();

        // Render the arrows.
        m_normalsArrowShader->bind();
        setMatrixUniforms(m_normalsArrowShader.get(), context);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        renderGeometry();
        m_normalsArrowShader->unbind();
    }
}

void PoolScene::renderGeometry()
{
    if (m_balls.size() == 0)
    {
        init();
    }

    if (m_sceneObjects.size() == 38)
    {
        for (int i = 0; i <= 15; i++)
        {
            SceneObject o = m_walls.at(i);
            glm::mat4 transform = o.composite;
            drawObject(o, transform, -1);
        }
        for (int i = 16; i <= 21; i++)
        {
            SceneObject o = m_holes.at(i - 16);
            glm::mat4 transform = o.composite;
            drawObject(o, transform, -1);
        }
        for (int i = 22; i <= 37; i++)
        {
            if (m_ball_done.at(i - 22))
            {
                continue;
            }
            SceneObject o = m_balls.at(i - 22);
            glm::mat4 transform = glm::translate(m_ball_translations.at(i - 22)) * o.composite;
            drawObject(o, transform, i - 22);
        }
    }
}

void PoolScene::drawObject(SceneObject o, glm::mat4 transform, int i)
{
    o.material.cAmbient *= m_ka;
    //    if(m_ball_velocities.size() == 16 && i != -1 ){
    //        o.material.cDiffuse = glm::vec4(glm::abs(m_ball_velocities.at(i)),0.f);
    //    }
    o.material.cDiffuse *= m_kd;
    m_phongShader->applyMaterial(o.material);
    m_phongShader->setUniform("m", transform);
    if (settings.drawNormals)
    {
        m_normalsShader->setUniform("m", transform);
        m_normalsArrowShader->setUniform("m", transform);
    }
    if (settings.drawWireframe)
    {
        m_wireframeShader->setUniform("m", transform);
    }
    applyTextureIfUsed(o);

    // draw shapes
    switch (o.primitive)
    {
    case PrimitiveType::PRIMITIVE_CUBE:
        m_cube->draw();
        break;
    case PrimitiveType::PRIMITIVE_CYLINDER:
        m_cylinder->draw();
        break;
    case PrimitiveType::PRIMITIVE_CONE:
        m_cone->draw();
        break;
    case PrimitiveType::PRIMITIVE_SPHERE:
        m_sphere->draw();
        break;
    case PrimitiveType::PRIMITIVE_MESH:
        break;
    case PrimitiveType::PRIMITIVE_TORUS:
        break;
    }
}

void PoolScene::setMatrixUniforms(Shader *shader, SupportCanvas3D *context)
{
    shader->setUniform("p", context->getCamera()->getProjectionMatrix());
    shader->setUniform("v", context->getCamera()->getViewMatrix());
}

void PoolScene::setLights()
{
    for (CS123SceneLightData light : m_lightData)
    {
        m_phongShader->setLight(light);
    }
}

void PoolScene::renderGeometry()
{
    if (m_object_translations.size() == 0)
    {
        init();
    }

    for (int i = 0; i < m_sceneObjects.size(); i++)
    {
        SceneObject o = m_sceneObjects[i];
        o.material.cAmbient *= m_ka;
        o.material.cDiffuse *= m_kd;
        glm::mat4 transform = glm::translate(m_object_translations.at(i)) * o.composite;
        m_phongShader->applyMaterial(o.material);
        m_phongShader->setUniform("m", transform);
        if (settings.drawNormals)
        {
            m_normalsShader->setUniform("m", transform);
            m_normalsArrowShader->setUniform("m", transform);
        }
        if (settings.drawWireframe)
        {
            m_wireframeShader->setUniform("m", transform);
        }

        // draw shapes
        switch (o.primitive)
        {
        case PrimitiveType::PRIMITIVE_CUBE:
            m_cube->draw();
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            m_cylinder->draw();
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            m_cone->draw();
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            m_sphere->draw();
            break;
        case PrimitiveType::PRIMITIVE_MESH:
            break;
        case PrimitiveType::PRIMITIVE_TORUS:
            break;
        }
    }
}

void PoolScene::tesselateShapes()
{
    m_cube = std::make_unique<CubeShape>();
    m_cone = std::make_unique<ConeShape>();
    m_cylinder = std::make_unique<CylinderShape>();
    m_sphere = std::make_unique<SphereShape>();
    m_cube->initializeShape(3, 3);
    m_cone->initializeShape(30, 30);
    m_cylinder->initializeShape(30, 30);
    m_sphere->initializeShape(30, 30);
}

void PoolScene::settingsChanged()
{
}

// This is just a filler method right now. We can change it to alter the
// objects in any way we need
void PoolScene::updateTranslation()
{
    for (int i = 0; i < m_object_translations.size(); i++)
    {
        glm::vec3 translate = glm::vec3(0.01f, 0.f, 0.f);
        m_object_translations[i] += translate;
    }
}
