#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
    void setup(float width, float height, float near, float far, float fovY);
    void setTransform(glm::vec3 pos, float pitch, float yaw);
    
    const glm::mat4 &view() const { return view_; }
    void setView(const glm::mat4 &v) { view_ = v; }
    const glm::mat4 &projection() const { return projection_; }
    glm::mat4 viewProjection() const { return projection_ * view_; }

    void clipOblique(glm::vec3 worldPos, glm::vec3 worldNorm);

    static glm::mat4 viewThroughPortal(
        const glm::mat4& playerWorld,
        const glm::mat4& srcWorld,
        const glm::mat4& dstWorld);

    float
        width = 0,    height = 0,
        near  = 0.1f, far    = 1000.0f,
        fovY  = glm::radians(60.0f);
private:
    glm::mat4 projection_{1.0f};
    glm::mat4 view_{1.0f};
};