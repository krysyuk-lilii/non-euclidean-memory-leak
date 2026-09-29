#include "core/scene.h"

GameObject& Scene::add(std::unique_ptr<GameObject> obj)
{
    obj->id = nextId_++;
    objects_.push_back(std::move(obj));
    return *objects_.back();
}

GameObject* Scene::find(uint64_t id)
{
    for (auto& o : objects_)
        if (o->id == id) return o.get();
    return nullptr;
}

std::unique_ptr<GameObject> Scene::release(GameObject* obj)
{
    for (auto it = objects_.begin(); it != objects_.end(); ++it)
    {
        if (it->get() == obj)
        {
            std::unique_ptr<GameObject> owned = std::move(*it);
            objects_.erase(it);
            return owned;
        }
    }
    return nullptr;
}

void Scene::update(double fixedDt)
{
    for (auto& o : objects_) o->update(fixedDt);
}

void Scene::render(const Camera& camera) const
{
    for (auto& o : objects_) o->render(camera);
}