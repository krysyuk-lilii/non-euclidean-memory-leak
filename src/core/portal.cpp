#include "core/portal.h"
#include "core/scene.h"
#include "core/game_object.h"
#include <cmath>
#include <memory>

Portal& createPortal(Scene& scene, glm::vec3 pos, float yaw, float radius)
{
    auto obj = std::make_unique<GameObject>();
    obj->pos = pos;
    obj->euler.y = yaw;

    auto portal = std::make_unique<Portal>();

    portal->obj        = &scene.add(std::move(obj));
    portal->radius     = radius;

    scene.portals.push_back(std::move(portal));
    return *scene.portals.back();
}
void linkPortals(Portal& a, Scene& sceneA, Portal& b, Scene& sceneB)
{
    a.linked = b.obj; a.linkedScene = &sceneB;
    b.linked = a.obj; b.linkedScene = &sceneA;
}
bool tryCrossPortal(PortalTraveler& t, const Portal& p)
{
    if (!p.linked) return false;

    glm::vec3 localOld = glm::vec3(p.obj->worldToLocal() * glm::vec4(t.prevPos, 1.0f));
    glm::vec3 localNew = glm::vec3(p.obj->worldToLocal() * glm::vec4(t.obj->pos, 1.0f));
    bool crossed = localOld.z > 0.0f && localNew.z <= 0.0f;
    bool withinBounds = (localNew.x * localNew.x + localNew.y * localNew.y) < (p.radius * p.radius);
    if (!crossed || !withinBounds) return false;

    glm::mat4 relative = p.linked->localToWorld() * p.obj->worldToLocal();
    glm::mat4 newWorld = relative * t.obj->localToWorld();

    t.obj->pos = glm::vec3(newWorld[3]);
    t.obj->euler.y = std::atan2(newWorld[2][0], newWorld[2][2]);
    t.velocity = glm::mat3(relative) * t.velocity;

    if (p.linkedScene && p.linkedScene != t.currScene)
    {
        auto owned = t.currScene->release(t.obj);
        t.obj = &p.linkedScene->add(std::move(owned));
        t.currScene = p.linkedScene;
    }
    return true;
}