#include <cstdio>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <unordered_set>

#include "components.hpp"
#include "entity.hpp"
#include "raylib.h"
#include "raymath.h"
#include "spatial_hash_map.hpp"
#include "renderables.hpp"

#define WINDOW_TITLE "CRAWLengine"

#define SCREEN_WIDTH (1280)
#define SCREEN_HEIGHT (720)

#define WORLD_WIDTH 100
#define WORLD_HEIGHT 100
#define TILE_SIZE 60.0f

#define PLAYER_SIZE 50.0f
#define PLAYER_SPAWN_X 2
#define PLAYER_SPAWN_Y 1
#define WALK_SPEED 120.0f
#define SPRINT_SPEED WALK_SPEED * 2

static const inline Rectangle GetCameraView(Camera2D &camera)
{
    float scale = 1.0f / camera.zoom;

    Rectangle view;
    view.x = camera.target.x - (camera.offset.x * scale);
    view.y = camera.target.y - (camera.offset.y * scale);
    view.width = SCREEN_WIDTH * scale;
    view.height = SCREEN_HEIGHT * scale;

    return view;
}

#define SPATIAL_UNIT 50

class Scene
{
  private:
    entt::registry registry;

    static const RenderableItem<Entity *> inline eToRI(Entity &entity)
    {
        return {&entity, entity.get<Tag::Renderable>().z};
    }

    static const RenderableItem<Entity *> inline eToRI(Entity &entity, int z)
    {
        return {&entity, z};
    }

    void onDestroyRenderable(entt::registry &registry, entt::entity e)
    {
        auto *entityP = get(e);
        if (entityP)
        {
            auto &entity = *entityP;

            renderables.remove(eToRI(entity));
        }
    }

    void onDestroyCollider(entt::registry &registry, entt::entity e)
    {
        auto *entityP = get(e);
        if (entityP)
        {
            auto &entity = *entityP;

            colliders.remove(&entity);
        }
    }

    void onConstructRenderable(entt::registry &registry, entt::entity e)
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

    void onConstructCollider(entt::registry &registry, entt::entity e)
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

    void onUpdateTransformForRenderable(Entity &entity)
    {
        auto &transform = entity.get<Component::Transform>();

        if (entity.has<Component::ColoredRect>())
        {
            auto &coloredRect = entity.get<Component::ColoredRect>();

            renderables.update(eToRI(entity),
                               {transform.pos.x, transform.pos.y, coloredRect.size.x, coloredRect.size.y});
        }
    }

    void onUpdateTransformForCollider(Entity &entity)
    {
        auto &transform = entity.get<Component::Transform>();
        auto &collider = entity.get<Component::Collider>();

        colliders.update(&entity, {transform.pos.x, transform.pos.y, collider.size.x, collider.size.y});
    }

    void onUpdateTransform(entt::registry &registry, entt::entity e)
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

    void onUpdateRenderable(entt::registry &registry, entt::entity e)
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
                    const auto renderable = eToRI(entity);

                    /* If you observe RenderableItem,
                     * we don't need the Z values to equal for a match.
                     * Thus we don't need the old one. */
                    renderables.replace(renderable, renderable);
                }
            }
        }
    }

  public:
    std::forward_list<Entity> entities;
    SpatialHashMap<RenderableItem<Entity *>> renderables;
    SpatialHashMap<Entity *> colliders;

    void inline setupRenderables()
    {
        registry.on_construct<Tag::Renderable>().connect<&Scene::onConstructRenderable>(this);
        registry.on_destroy<Tag::Renderable>().connect<&Scene::onDestroyRenderable>(this);
        registry.on_update<Tag::Renderable>().connect<&Scene::onUpdateRenderable>(this);
    }

    void inline setupColliders()
    {
        registry.on_construct<Component::Collider>().connect<&Scene::onConstructCollider>(this);
        registry.on_destroy<Component::Collider>().connect<&Scene::onDestroyCollider>(this);
    }

    Scene()
        : renderables(std::make_pair(SPATIAL_UNIT, SPATIAL_UNIT)), colliders(std::make_pair(SPATIAL_UNIT, SPATIAL_UNIT))
    {
        /* TODO: reactive storage
         * https://github.com/skypjack/entt/wiki/Entity-Component-System#storage */
        setupRenderables();
        setupColliders();

        registry.on_update<Component::Transform>().connect<&Scene::onUpdateTransform>(this);
    }

    Entity &add()
    {
        return entities.emplace_front(&registry);
    }

    Entity *get(entt::entity e)
    {
        auto it =
            std::find_if(entities.begin(), entities.end(), [&e](auto &entity) { return (entt::entity)entity == e; });

        if (it == entities.end())
        {
            return nullptr;
        }

        return &(*it);
    }

    void remove(Entity &entity)
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

    template <typename... ComponentTypes, typename Func> void eachE(Func &&func)
    {
        registry.template view<ComponentTypes...>().template each(
            [this, func = std::forward<Func>(func)](auto e, auto &...components) {
                auto *entityP = get(e);
                if (entityP)
                {
                    auto &entity = *entityP;

                    func(entity, components...);
                }
            });
    }

    template <typename... ComponentTypes, typename Func> void each(Func &&func)
    {
        registry.template view<ComponentTypes...>().template each(
            [func = std::forward<Func>(func)](auto, auto &...components) { func(components...); });
    }
};

static void drawRenderables(Camera2D &cam, SpatialHashMap<RenderableItem<Entity *>> &renderables)
{
    auto view = GetCameraView(cam);

    view.x -= TILE_SIZE;
    view.y -= TILE_SIZE;
    view.width += 3 * TILE_SIZE;
    view.height += 3 * TILE_SIZE;

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

static const inline std::array<Rectangle, 8> NeighbouringColliders(Scene &scene, Rectangle collider)
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

static bool IsColliding(Scene &scene, Rectangle collider, Rectangle &result)
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

static const inline Rectangle tcToRect(const Component::Transform &transform, const Component::Collider &collider)
{
    return {transform.pos.x, transform.pos.y, collider.size.x, collider.size.y};
}

static const inline Rectangle taToRect(const Component::Transform &transform, const Component::Area &area)
{
    return {transform.pos.x, transform.pos.y, area.size.x, area.size.y};
}

namespace Event
{
    struct Enter
    {
        Entity *entity;
    };
}

namespace System
{
    class System
    {
      public:
        virtual void tick(Scene &scene) = 0;
        virtual ~System() = default;
    };

    class RenderToCameras : public System
    {
        void tick(Scene &scene) override
        {
            auto &renderables = scene.renderables;

            scene.each<Component::Camera>([&renderables](auto &comp) {
                auto &cam = comp.cam;

                BeginMode2D(cam);
                drawRenderables(cam, renderables);
                EndMode2D();
            });
        }
    };

    class FollowCameraTargets : public System
    {
        void tick(Scene &scene) override
        {
            scene.each<Component::Camera>([](auto &comp) {
                auto &cam = comp.cam;
                auto *entityP = comp.target;

                if (entityP)
                {
                    auto &entity = *entityP;

                    if (entity.template has<Component::Transform>())
                    {
                        cam.target = Vector2Add(entity.template get<Component::Transform>(), comp.offset);
                    }
                }
            });
        }
    };

    class Locomotion : public System
    {
        void tick(Scene &scene) override
        {
            scene.eachE<Component::Transform, Component::Locomotion, Component::Collider>(
                [&scene](auto &entity, const auto &transform, const auto &locomotion, const auto &collider) {
                    Rectangle collision;

                    if (Vector2LengthSqr(locomotion.vel) != 0)
                    {
                        entity.template update<Component::Transform>(
                            [&locomotion](auto &transform) { transform.pos.x += locomotion.vel.x; });

                        if (IsColliding(scene, tcToRect(transform, collider), collision))
                        {
                            entity.template update<Component::Transform>([&collision, &locomotion](auto &transform) {
                                transform.pos.x -= collision.width * ((locomotion.vel.x > 0) - (locomotion.vel.x < 0));
                            });
                        }

                        entity.template update<Component::Transform>(
                            [&locomotion](auto &transform) { transform.pos.y += locomotion.vel.y; });

                        if (IsColliding(scene, tcToRect(transform, collider), collision))
                        {
                            entity.template update<Component::Transform>([&collision, &locomotion](auto &transform) {
                                transform.pos.y -= collision.height * ((locomotion.vel.y > 0) - (locomotion.vel.y < 0));
                            });
                        }
                    }
                });
        }
    };

    namespace Steering
    {
        class Test : public System
        {
            void tick(Scene &scene) override
            {
                scene.each<Component::Steering::Test, Component::Locomotion>([](const auto &test, auto &locomotion) {
                    locomotion.targetSpeed = test.speed;
                    Vector2 in;

                    if ((int)GetTime() % 2 == 0)
                    {
                        in = {1, 0};
                    }
                    else if ((int)GetTime() % 3 == 0)
                    {
                        in = {0, 1};
                    }
                    else if ((int)GetTime() % 8 == 0)
                    {
                        in = {-1, 0};
                    }
                    else
                    {
                        in = {0, -1};
                    }

                    locomotion.setVelocity(Vector2Scale(in, locomotion.targetSpeed));
                });
            }
        };

        class Player : public System
        {
          public:
            void tick(Scene &scene) override
            {
                scene.each<Component::Steering::Player, Component::Locomotion>([](const auto &input, auto &locomotion) {
                    /* input will be used when`
                     * I get into multiple input devices,
                     * custom keybinds and many more advanced ideas... */

                    if (IsKeyDown(KEY_LEFT_SHIFT))
                    {
                        locomotion.targetSpeed = SPRINT_SPEED;
                    }
                    else
                    {
                        locomotion.targetSpeed = WALK_SPEED;
                    }
                    Vector2 in = {(float)(IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT)),
                                  (float)(IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP))};

                    locomotion.setVelocity(Vector2Scale(in, locomotion.targetSpeed));
                });
            }
        };
    }

    class Area : public System
    {
        void tick(Scene &scene) override
        {
            scene.each<Component::Area, Component::Transform>([&scene](auto &area, const auto &transform) {
                const Rectangle search = {transform.pos.x - area.size.x, transform.pos.y - area.size.y, area.size.x * 3,
                                          area.size.y * 3};
                auto entities = scene.colliders.nearby(search);

                for (auto &entityP : entities)
                {
                    if (entityP)
                    {
                        auto &entity = *entityP;

                        if (entity.has<Component::Transform, Component::Collider>())
                        {
                            auto &eTransform = entity.get<Component::Transform>();
                            auto &eCollider = entity.get<Component::Collider>();
                            const auto result =
                                GetCollisionRec(taToRect(transform, area), tcToRect(eTransform, eCollider));

                            /* FIXME: on the right side of the area there is a
                             * deadzone? */
                            if (result.width > 0 && result.height > 0)
                            {
                                area.dispatcher.template trigger<Event::Enter>(Event::Enter{entityP});
                            }
                        }
                    }
                }
            });
        }
    };
}

void addWorld(Scene &scene)
{
    for (int x = 0; x < WORLD_WIDTH; x++)
    {
        for (int y = 0; y < WORLD_HEIGHT; y++)
        {
            auto &entity = scene.add();

            if (x == 0 || x == WORLD_WIDTH - 1 || y == 0 || y == WORLD_HEIGHT - 1 || (x == 5 && (y > 3 && y < 9)) ||
                (x == 8 && (y > 3 && y < 9)))
            {
                entity.add<Component::Transform>(Vector2{x * TILE_SIZE, y * TILE_SIZE});
                entity.add<Component::Collider>(Vector2{TILE_SIZE, TILE_SIZE});
                entity.add<Component::ColoredRect>(GRAY, Vector2{TILE_SIZE, TILE_SIZE});
                entity.add<Tag::Renderable>();
            }
            else if (x == 10)
            {
                entity.add<Component::Transform>(Vector2{x * TILE_SIZE, y * TILE_SIZE});
                entity.add<Component::ColoredRect>(DARKBLUE, Vector2{TILE_SIZE, TILE_SIZE});
                entity.add<Tag::Renderable>(-10);

                entity.add<Component::Area>(Vector2{TILE_SIZE, TILE_SIZE});
            }
            else
            {
                entity.add<Component::Transform>(Vector2{x * TILE_SIZE, y * TILE_SIZE});
                entity.add<Component::ColoredRect>(DARKGRAY, Vector2{TILE_SIZE, TILE_SIZE});
                entity.add<Tag::Renderable>(-10);
            }
        }
    }
}

void addPlayer(Scene &scene)
{
    auto &entity = scene.add();

    entity.add<Component::Transform>(Vector2{PLAYER_SPAWN_X * TILE_SIZE, PLAYER_SPAWN_Y * TILE_SIZE});
    entity.add<Component::Locomotion>();
    entity.add<Component::Collider>(Vector2{PLAYER_SIZE, PLAYER_SIZE});
    entity.add<Component::Steering::Player>();

    entity.add<Component::ColoredRect>(GREEN, Vector2{PLAYER_SIZE, PLAYER_SIZE});
    entity.add<Tag::Renderable>();

    entity.add<Component::Camera>();
    Component::Camera &comp = entity.get<Component::Camera>();
    Camera2D *cam = &comp.cam;

    comp.target = &entity;
    comp.offset = {PLAYER_SIZE / 2, PLAYER_SIZE / 2};

    cam->target = {0, 0};
    cam->offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    cam->rotation = 0.0f;
    cam->zoom = 1.0f;
}

void addEnemy(Scene &scene)
{
    auto &entity = scene.add();

    entity.add<Component::Transform>(Vector2{3 * TILE_SIZE, 4 * TILE_SIZE});
    entity.add<Component::Collider>(Vector2{PLAYER_SIZE, PLAYER_SIZE});

    entity.add<Component::Locomotion>();

    entity.add<Component::Steering::Test>();
    entity.get<Component::Steering::Test>().speed = WALK_SPEED;

    entity.add<Component::ColoredRect>(RED, Vector2{PLAYER_SIZE, PLAYER_SIZE});
    entity.add<Tag::Renderable>();
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    Scene scene;

    std::vector<std::unique_ptr<System::System>> systems;
    {
        using namespace System;

        systems.push_back(std::make_unique<Steering::Player>());
        systems.push_back(std::make_unique<Steering::Test>());

        systems.push_back(std::make_unique<Locomotion>());

        systems.push_back(std::make_unique<Area>());

        systems.push_back(std::make_unique<FollowCameraTargets>());
        systems.push_back(std::make_unique<RenderToCameras>());
    }

    addWorld(scene);
    addPlayer(scene);
    addEnemy(scene);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (auto &system : systems)
        {
            system->tick(scene);
        }

        DrawFPS(0, 0);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
