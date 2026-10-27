// src/render/Portal.h
#pragma once
#include "render/frame_buffer.h"
#include <glm/glm.hpp>
class GameObject;
class Scene;
struct Portal
{
    float radius = 1.0f;
    GameObject *obj         = nullptr;
    GameObject *linked      = nullptr;
    Scene      *linkedScene = nullptr;
};
struct PortalTraveler
{
    GameObject *obj       = nullptr;
    Scene      *currScene = nullptr;

    glm::vec3 velocity{0.0f};
	glm::vec3 prevPos{0.0f};
};

Portal& createPortal(Scene& scene, glm::vec3 pos, float yaw, float radius);
void linkPortals(Portal& a, Scene& sceneA, Portal& b, Scene& sceneB);
bool tryCrossPortal(PortalTraveler& t, const Portal& p);