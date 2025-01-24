
#include "renderables.hpp"
#include "components/components.hpp"
#include "entity.hpp"
#include "scene.hpp"

#define SPATIAL_UNIT 50

const RenderableItem<Entity *> Renderables::eToRI(Entity &entity, int z)
{
    return {&entity, z};
}

const RenderableItem<Entity *> Renderables::eToRI(Entity &entity)
{
    return eToRI(entity, entity.get<Tags::Renderable>().z);
}

Renderables::Renderables(Scene *scene) : items(std::make_pair(SPATIAL_UNIT, SPATIAL_UNIT))
{
    scene->onConstruct<Tags::Renderable>([this](auto &entity) {
        if (entity.template has<Components::Transform>())
        {
            auto &transform = entity.template get<Components::Transform>();

            if (entity.template has<Components::ColoredRect>())
            {
                auto &coloredRect = entity.template get<Components::ColoredRect>();
                items.insert(eToRI(entity), {transform.pos.x, transform.pos.y, coloredRect.size.x, coloredRect.size.y});
            }
        }
    });

    scene->onUpdate<Tags::Renderable>([this](auto &entity) {
        if (entity.template has<Components::Transform>())
        {
            auto &transform = entity.template get<Components::Transform>();

            if (entity.template has<Components::ColoredRect>())
            {
                const auto renderable = eToRI(entity);
                items.replace(renderable, renderable);
            }
        }
    });

    scene->onDestroy<Tags::Renderable>([this](auto &entity) { items.remove(eToRI(entity)); });

    scene->onUpdate<Components::Transform>([this](auto &entity) {
        auto &transform = entity.template get<Components::Transform>();

        if (entity.template has<Tags::Renderable>())
        {
            if (entity.template has<Components::ColoredRect>())
            {
                auto &coloredRect = entity.template get<Components::ColoredRect>();
                items.update(eToRI(entity),
                             Rectangle{transform.pos.x, transform.pos.y, coloredRect.size.x, coloredRect.size.y});
            }
        }
    });
}

Rectangle Renderables::enlarge(Rectangle rect)
{
    rect.x -= rect.width / 2;
    rect.y -= rect.height / 2;
    rect.width *= 2;
    rect.height *= 2;

    return rect;
}

void Renderables::insert(RenderableItem<Entity *> item, Rectangle rect)
{
    items.insert(item, rect);
}

void Renderables::update(RenderableItem<Entity *> item, Rectangle rect)
{
    items.update(item, rect);
}

void Renderables::replace(RenderableItem<Entity *> item, RenderableItem<Entity *> t)
{
    items.replace(item, t);
}

void Renderables::remove(RenderableItem<Entity *> item)
{
    items.remove(item);
}

SpacePartitioners::SpatialCell<RenderableItem<Entity *>> Renderables::nearby(Rectangle rect)
{
    return items.nearby(enlarge(rect));
}
