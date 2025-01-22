#include "scene.hpp"

SceneCallback::SceneCallback(Scene *s, EntityCallback f) : scene(s), func(f) {};

void SceneCallback::connect(entt::registry &registry, entt::entity e)
{
    auto *entityP = scene->get(e);
    if (entityP)
    {
        func(*entityP);
    }
}

#define SPATIAL_UNIT 50

Scene::Scene() : renderables(this), colliders(this) {}

Entity &Scene::add()
{
    return entities.emplace_front(&registry);
}

Entity *Scene::get(entt::entity e)
{
    auto it = std::find_if(entities.begin(), entities.end(),
                           [&e](auto &entity) { return static_cast<entt::entity>(entity) == e; });

    return (it != entities.end()) ? &(*it) : nullptr;
}

void Scene::remove(Entity &entity)
{
    auto prev = entities.before_begin();
    for (auto it = entities.begin(); it != entities.end(); ++it)
    {
        if (*it == entity)
        {
            entities.erase_after(prev);
            return;
        }
        ++prev;
    }
}
