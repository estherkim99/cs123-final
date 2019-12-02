#include "CS123SceneData.h"
#include "SupportCanvas2D.h"

#ifndef LISTNODE_H
#define LISTNODE_H

class ListNode
{
public:
    ListNode(CS123ScenePrimitive primitive, const glm::mat4x4 &matrix);
    ~ListNode();

    CS123ScenePrimitive m_object;
    glm::mat4x4 m_transformation;
    QImage m_texture;
};

#endif // LISTNODE_H
