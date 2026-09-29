#pragma once
#include <glm/glm.hpp>
#include <functional>
#include <cstdint>
class Camera;

class GameObject
{
public:
    uint64_t id = 0;
    glm::vec3 pos{0.0f};
    glm::vec3 euler{0.0f}; // pitch (x), yaw (y), roll (z), radians
    glm::vec3 scale{1.0f};

    std::function<void(double fixedDt)> onUpdate;
    std::function<void(const Camera&)> onRender;

    glm::vec3 forward() const;
    glm::mat4 localToWorld() const;
    glm::mat4 worldToLocal() const;

    void update(double fixedDt) { if (onUpdate) onUpdate(fixedDt); }
    void render(const Camera& camera) const { if (onRender) onRender(camera); }
};
