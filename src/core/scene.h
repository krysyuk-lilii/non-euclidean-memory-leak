#pragma once
#include "core/game_object.h"
#include "core/portal.h"
#include <cstdint>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
class Camera;

class Scene
{
public:
    GameObject& add(std::unique_ptr<GameObject> obj);
    GameObject* find(uint64_t id);

    // Hands ownership of `obj` back to the caller (for moving a traveler into
    // another Scene). Returns nullptr if `obj` isn't in this scene.
    std::unique_ptr<GameObject> release(GameObject* obj);

    void update(double fixedDt);
    void render(const Camera& camera) const;

    // Portals that live in this scene. unique_ptr keeps each Portal's address
    // stable as the vector grows (PortalRenderer keys FBOs by Portal*).
    std::vector<std::unique_ptr<Portal>> portals;

    glm::vec4 backgroundColor{0.0f, 0.0f, 0.0f, 1.0f};

private:
    std::vector<std::unique_ptr<GameObject>> objects_;
    uint64_t nextId_ = 1;
};