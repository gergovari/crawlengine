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

void Scene::setupColliders()
{
    onConstruct<Component::Collider>([this](auto &entity) {
        if (entity.template has<Component::Transform>())
        {
            auto &transform = entity.template get<Component::Transform>();
            auto &collider = entity.template get<Component::Collider>();

            colliders.insert(&entity, {transform.pos.x, transform.pos.y, collider.size.x, collider.size.y});
        }
    });

    onDestroy<Component::Collider>([this](auto &entity) { colliders.remove(&entity); });
}

void Scene::setupTransforms()
{
    onUpdate<Component::Transform>([this](auto &entity) {
        if (entity.template has<Component::Collider>())
        {
            auto &transform = entity.template get<Component::Transform>();
            auto &collider = entity.template get<Component::Collider>();

            colliders.update(&entity, {transform.pos.x, transform.pos.y, collider.size.x, collider.size.y});
        }
    });
}

Scene::Scene() : renderables(this), colliders(std::make_pair(SPATIAL_UNIT, SPATIAL_UNIT))
{
    setupColliders();
    setupTransforms();
}

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
