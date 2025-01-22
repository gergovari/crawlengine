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

const RenderableItem<Entity *> Scene::eToRI(Entity &entity)
{
    return {&entity, entity.get<Tag::Renderable>().z};
}

const RenderableItem<Entity *> Scene::eToRI(Entity &entity, int z)
{
    return {&entity, z};
}

void Scene::setupRenderables()
{
    onConstruct<Tag::Renderable>([this](auto &entity) {
        if (entity.template has<Component::Transform>())
        {
            auto &transform = entity.template get<Component::Transform>();

            if (entity.template has<Component::ColoredRect>())
            {
                auto &coloredRect = entity.template get<Component::ColoredRect>();
                renderables.insert(eToRI(entity),
                                   {transform.pos.x, transform.pos.y, coloredRect.size.x, coloredRect.size.y});
            }
        }
    });

    onUpdate<Tag::Renderable>([this](auto &entity) {
        if (entity.template has<Component::Transform>())
        {
            auto &transform = entity.template get<Component::Transform>();

            if (entity.template has<Component::ColoredRect>())
            {
                const auto renderable = eToRI(entity);
                renderables.replace(renderable, renderable);
            }
        }
    });

    onDestroy<Tag::Renderable>([this](auto &entity) { renderables.remove(eToRI(entity)); });
}

void Scene::setupTransforms()
{
    onUpdate<Component::Transform>([this](auto &entity) {
        auto &transform = entity.template get<Component::Transform>();

        if (entity.template has<Tag::Renderable>())
        {
            if (entity.template has<Component::ColoredRect>())
            {
                auto &coloredRect = entity.template get<Component::ColoredRect>();
                renderables.update(eToRI(entity),
                                   Rectangle{transform.pos.x, transform.pos.y, coloredRect.size.x, coloredRect.size.y});
            }
        }
    });

    onUpdate<Component::Transform>([this](auto &entity) {
        if (entity.template has<Component::Collider>())
        {
            auto &transform = entity.template get<Component::Transform>();
            auto &collider = entity.template get<Component::Collider>();

            colliders.update(&entity, {transform.pos.x, transform.pos.y, collider.size.x, collider.size.y});
        }
    });
}

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

#define SPATIAL_UNIT 50

Scene::Scene()
    : renderables(std::make_pair(SPATIAL_UNIT, SPATIAL_UNIT)), colliders(std::make_pair(SPATIAL_UNIT, SPATIAL_UNIT))
{
    setupRenderables();
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
