#include "collidables.hpp"

#include "components/components.hpp"
#include "entity.hpp"
#include "scene.hpp"

#define SPATIAL_UNIT 50

Collidables::Collidables(Scene *scene) : items(std::make_pair(SPATIAL_UNIT, SPATIAL_UNIT))
{
    scene->onConstruct<Components::Collider>([this](auto &entity) {
        if (entity.template has<Components::Transform>())
        {
            auto &transform = entity.template get<Components::Transform>();
            auto &collider = entity.template get<Components::Collider>();

            items.insert(&entity, {transform.pos.x, transform.pos.y, collider.size.x, collider.size.y});
        }
    });

    scene->onDestroy<Components::Collider>([this](auto &entity) { items.remove(&entity); });

    scene->onUpdate<Components::Transform>([this](auto &entity) {
        if (entity.template has<Components::Collider>())
        {
            auto &transform = entity.template get<Components::Transform>();
            auto &collider = entity.template get<Components::Collider>();

            items.update(&entity, {transform.pos.x, transform.pos.y, collider.size.x, collider.size.y});
        }
    });
}

Rectangle Collidables::enlarge(Rectangle rect)
{
    rect.x -= rect.width / 2;
    rect.y -= rect.height / 2;
    rect.width *= 2;
    rect.height *= 2;

    return rect;
}

void Collidables::insert(Entity *item, Rectangle rect)
{
    items.insert(item, rect);
}

void Collidables::update(Entity *item, Rectangle rect)
{
    items.update(item, rect);
}

void Collidables::replace(Entity *item, Entity *t)
{
    items.replace(item, t);
}

void Collidables::remove(Entity *item)
{
    items.remove(item);
}

SpacePartitioners::SpatialCell<Entity *> Collidables::nearby(Rectangle rect)
{
    return items.nearby(enlarge(rect));
}
