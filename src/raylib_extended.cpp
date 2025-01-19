#include "raylib_extended.hpp"
#include "components.hpp"

Rectangle GetCameraView(Camera2D &camera)
{
    float scale = 1.0f / camera.zoom;

    Rectangle view;
    view.x = camera.target.x - (camera.offset.x * scale);
    view.y = camera.target.y - (camera.offset.y * scale);
    view.width = GetScreenWidth() * scale;
    view.height = GetScreenHeight() * scale;

    return view;
}

void DrawRenderables(Camera2D &cam, SpatialHashMap<RenderableItem<Entity *>> &renderables)
{
    auto view = GetCameraView(cam);

    view.x -= view.width / 2;
    view.y -= view.height / 2;
    view.width *= 2;
    view.height *= 2;

    auto items = renderables.nearby(view);

    items.sort([](auto const &a, auto const &b) { return a.z < b.z; });

    for (auto &renderable : items)
    {
        Entity &entity = *renderable.item;
        auto &transform = entity.get<Component::Transform>();

        if (entity.has<Component::ColoredRect>())
        {
            auto &rect = entity.get<Component::ColoredRect>();

            DrawRectangleV(transform.pos, rect.size, rect.color);
        }
    }
}

std::array<Rectangle, 8> NeighbouringColliders(Scene &scene, Rectangle collider)
{
    const Vector2 transform = {collider.x + collider.width / 2, collider.y + collider.height / 2};
    const std::array<Vector2, 8> offsets = {{{-collider.width, -collider.height},
                                             {-collider.width, 0},
                                             {-collider.width, collider.height},
                                             {0, -collider.height},
                                             {0, collider.height},
                                             {collider.width, -collider.height},
                                             {collider.width, 0},
                                             {collider.width, collider.height}}};
    std::array<Rectangle, 8> found;
    size_t count = 0;

    collider = {collider.x - collider.width, collider.y - collider.height, collider.width * 3, collider.height * 3};
    auto entities = scene.colliders.nearby(collider);

    for (auto &entity : entities)
    {
        if (entity)
        {
            auto &tilePos = entity->get<Component::Transform>();
            auto &collider = entity->get<Component::Collider>();

            for (const auto &offset : offsets)
            {
                const Rectangle other = {tilePos.pos.x, tilePos.pos.y, collider.size.x, collider.size.y};

                if (CheckCollisionPointRec(Vector2Add(transform, offset), other))
                {
                    if (found.size() <= count)
                    {
                        return found;
                    }
                    found[count++] = other;
                }
            }
        }
    }
    return found;
}

bool IsColliding(Scene &scene, Rectangle collider, Rectangle &result)
{
    const auto found = NeighbouringColliders(scene, collider);

    for (const auto &collision : found)
    {
        result = GetCollisionRec(collider, collision);
        if (result.width > 0 && result.height > 0)
        {
            return true;
        }
    }
    return false;
}

Rectangle TcToRect(const Component::Transform &transform, const Component::Collider &collider)
{
    return {transform.pos.x, transform.pos.y, collider.size.x, collider.size.y};
}

Rectangle TaToRect(const Component::Transform &transform, const Component::Area &area)
{
    return {transform.pos.x, transform.pos.y, area.size.x, area.size.y};
}
