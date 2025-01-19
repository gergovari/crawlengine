#include "scene.hpp"

const RenderableItem<Entity *> Scene::eToRI(Entity &entity)
{
    return {&entity, entity.get<Tag::Renderable>().z};
}

const RenderableItem<Entity *> Scene::eToRI(Entity &entity, int z)
{
    return {&entity, z};
}

void Scene::onDestroyRenderable(entt::registry &registry, entt::entity e)
{
    auto *entityP = get(e);
    if (entityP)
    {
        auto &entity = *entityP;
        renderables.remove(eToRI(entity));
    }
}

void Scene::onDestroyCollider(entt::registry &registry, entt::entity e)
{
    auto *entityP = get(e);
    if (entityP)
    {
        auto &entity = *entityP;
        colliders.remove(&entity);
    }
}

void Scene::onConstructRenderable(entt::registry &registry, entt::entity e)
{
    auto *entityP = get(e);
    if (entityP)
    {
        auto &entity = *entityP;

        if (entity.has<Component::Transform>())
        {
            auto &transform = entity.get<Component::Transform>();

            if (entity.has<Component::ColoredRect>())
            {
                auto &coloredRect = entity.get<Component::ColoredRect>();
                renderables.insert(eToRI(entity),
                                   {transform.pos.x, transform.pos.y, coloredRect.size.x, coloredRect.size.y});
            }
        }
    }
}

void Scene::onConstructCollider(entt::registry &registry, entt::entity e)
{
    auto *entityP = get(e);
    if (entityP)
    {
        auto &entity = *entityP;

        if (entity.has<Component::Transform>())
        {
            auto &transform = entity.get<Component::Transform>();
            auto &collider = entity.get<Component::Collider>();

            colliders.insert(&entity, {transform.pos.x, transform.pos.y, collider.size.x, collider.size.y});
        }
    }
}

void Scene::onUpdateTransformForRenderable(Entity &entity)
{
    auto &transform = entity.get<Component::Transform>();

    if (entity.has<Component::ColoredRect>())
    {
        auto &coloredRect = entity.get<Component::ColoredRect>();
        renderables.update(eToRI(entity), {transform.pos.x, transform.pos.y, coloredRect.size.x, coloredRect.size.y});
    }
}

void Scene::onUpdateTransformForCollider(Entity &entity)
{
    auto &transform = entity.get<Component::Transform>();
    auto &collider = entity.get<Component::Collider>();

    colliders.update(&entity, {transform.pos.x, transform.pos.y, collider.size.x, collider.size.y});
}

void Scene::onUpdateTransform(entt::registry &registry, entt::entity e)
{
    auto *entityP = get(e);
    if (entityP)
    {
        auto &entity = *entityP;

        if (entity.has<Tag::Renderable>())
        {
            onUpdateTransformForRenderable(entity);
        }

        if (entity.has<Component::Collider>())
        {
            onUpdateTransformForCollider(entity);
        }
    }
}

void Scene::onUpdateRenderable(entt::registry &registry, entt::entity e)
{
    auto *entityP = get(e);
    if (entityP)
    {
        auto &entity = *entityP;

        if (entity.has<Component::Transform>())
        {
            auto &transform = entity.get<Component::Transform>();

            if (entity.has<Component::ColoredRect>())
            {
                const auto renderable = eToRI(entity);
                renderables.replace(renderable, renderable);
            }
        }
    }
}

void Scene::setupRenderables()
{
    registry.on_construct<Tag::Renderable>().connect<&Scene::onConstructRenderable>(this);
    registry.on_destroy<Tag::Renderable>().connect<&Scene::onDestroyRenderable>(this);
    registry.on_update<Tag::Renderable>().connect<&Scene::onUpdateRenderable>(this);
}

void Scene::setupColliders()
{
    registry.on_construct<Component::Collider>().connect<&Scene::onConstructCollider>(this);
    registry.on_destroy<Component::Collider>().connect<&Scene::onDestroyCollider>(this);
}

#define SPATIAL_UNIT 50

Scene::Scene()
    : renderables(std::make_pair(SPATIAL_UNIT, SPATIAL_UNIT)), colliders(std::make_pair(SPATIAL_UNIT, SPATIAL_UNIT))
{
    setupRenderables();
    setupColliders();
    registry.on_update<Component::Transform>().connect<&Scene::onUpdateTransform>(this);
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
