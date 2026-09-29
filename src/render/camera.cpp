#include "render/camera.h"
#include <glm/gtc/matrix_transform.hpp>

void Camera::setup(float w, float h, float n, float f, float fovYRad)
{
    width       = w;
    height      = h;
    near        = n;
    far         = f;
    fovY        = fovYRad;
    projection_ = glm::perspective(fovY, width / height, near, far);
}
void Camera::setTransform(glm::vec3 pos, float pitch, float yaw)
{
    glm::mat4 m(1.0f);
    m = glm::rotate(m, pitch, glm::vec3(1, 0, 0));
    m = glm::rotate(m, yaw,   glm::vec3(0, 1, 0));
    m = glm::translate(m, -pos);

    view_ = m;
}
void Camera::clipOblique(glm::vec3 worldPos, glm::vec3 worldNorm)
{
    glm::vec4 cPos     = view_ * glm::vec4(worldPos, 1.0f);
    glm::vec4 cNormDir = view_ * glm::vec4(worldNorm, 0.0f);
    glm::vec3 cNorm    = glm::normalize(glm::vec3(cNormDir));

    glm::vec4 clipPlane(cNorm, -glm::dot(glm::vec3(cPos), cNorm));
    glm::vec4 q(
        (glm::sign(clipPlane.x) + projection_[2][0]) / projection_[0][0],
        (glm::sign(clipPlane.y) + projection_[2][1]) / projection_[1][1],
        -1.0f,
        (1.0f + projection_[2][2]) / projection_[3][2]);

    glm::vec4 c = clipPlane * (2.0f / glm::dot(clipPlane, q));

    projection_[0][2] = c.x;
    projection_[1][2] = c.y;
    projection_[2][2] = c.z + 1.0f;
    projection_[3][2] = c.w;
}
// Camera.cpp
glm::mat4 Camera::viewThroughPortal(
    const glm::mat4& playerWorld,
    const glm::mat4& srcWorld,
    const glm::mat4& dstWorld)
{
    glm::mat4 virtualWorld = dstWorld * glm::inverse(srcWorld) * playerWorld;
    return glm::inverse(virtualWorld);
}