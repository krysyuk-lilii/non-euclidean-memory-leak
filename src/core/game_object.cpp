#include "core/game_object.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

glm::vec3 GameObject::forward() const
{
    glm::mat4 rot = glm::eulerAngleZXY(euler.z, euler.x, euler.y);
    // -Z axis of the rotation matrix, matching the original's
    // "negate the matrix's z axis" convention.
    return -glm::vec3(rot[0][2], rot[1][2], rot[2][2]);
}

glm::mat4 GameObject::localToWorld() const
{
    glm::mat4 m = glm::translate(glm::mat4(1.0f), pos);
    m = glm::rotate(m, euler.y, glm::vec3(0, 1, 0));
    m = glm::rotate(m, euler.x, glm::vec3(1, 0, 0));
    m = glm::rotate(m, euler.z, glm::vec3(0, 0, 1));
    m = glm::scale(m, scale);
    return m;
}

glm::mat4 GameObject::worldToLocal() const
{
    return glm::inverse(localToWorld());
}