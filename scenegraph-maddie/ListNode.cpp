#include "ListNode.h"
#include "CS123SceneData.h"

ListNode::ListNode(CS123ScenePrimitive primitive, const glm::mat4x4 &matrix) :
    m_object(primitive),
    m_transformation(matrix),
    m_texture(QImage::QImage())
{
}

ListNode::~ListNode()
{
}
