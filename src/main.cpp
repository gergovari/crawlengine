#include <cstdio>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <unordered_set>

#include "components/components.hpp"
#include "entity/entity.hpp"
#include "events/events.hpp"
#include "raymath.h"
#include "misc/raylib_extended.hpp"
#include "scene/renderables.hpp"
#include "scene/scene.hpp"
#include "systems/systems.hpp"

#define WINDOW_TITLE "CRAWLengine"

#define SCREEN_WIDTH (1280)
#define SCREEN_HEIGHT (720)

#define WORLD_WIDTH 100
#define WORLD_HEIGHT 100
#define TILE_SIZE 60.0f

#define PLAYER_SIZE 50.0f
#define PLAYER_SPAWN_X 2
#define PLAYER_SPAWN_Y 1

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
                entity.add<Components::Transform>(Vector2{x * TILE_SIZE, y * TILE_SIZE});
                entity.add<Components::Collider>(Vector2{TILE_SIZE, TILE_SIZE});
                entity.add<Components::ColoredRect>(GRAY, Vector2{TILE_SIZE, TILE_SIZE});
                entity.add<Tags::Renderable>();
            }
            else if (x == 10)
            {
                entity.add<Components::Transform>(Vector2{x * TILE_SIZE, y * TILE_SIZE});
                entity.add<Components::ColoredRect>(DARKBLUE, Vector2{TILE_SIZE, TILE_SIZE});
                entity.add<Tags::Renderable>(-9);

                entity.add<Components::Area>((size_t)0, Vector2{TILE_SIZE, TILE_SIZE});
                entity.add<Components::Locomotion::Multiplier>();
            }
            else
            {
                entity.add<Components::Transform>(Vector2{x * TILE_SIZE, y * TILE_SIZE});
                entity.add<Components::ColoredRect>(DARKGRAY, Vector2{TILE_SIZE, TILE_SIZE});
                entity.add<Tags::Renderable>(-10);
            }
        }
    }
}

void addPlayer(Scene &scene)
{
    auto &entity = scene.add();

    entity.add<Components::Transform>(Vector2{PLAYER_SPAWN_X * TILE_SIZE, PLAYER_SPAWN_Y * TILE_SIZE});
    entity.add<Components::Locomotion::Velocity>();
    entity.add<Components::Collider>(Vector2{PLAYER_SIZE, PLAYER_SIZE});
    entity.add<Components::Steering::Player>();

    entity.add<Components::ColoredRect>(GREEN, Vector2{PLAYER_SIZE, PLAYER_SIZE});
    entity.add<Tags::Renderable>();

    entity.add<Components::Camera>();
    Components::Camera &comp = entity.get<Components::Camera>();
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

    entity.add<Components::Transform>(Vector2{3 * TILE_SIZE, 4 * TILE_SIZE});
    entity.add<Components::Collider>(Vector2{PLAYER_SIZE, PLAYER_SIZE});

    entity.add<Components::Locomotion::Velocity>();

    entity.add<Components::Steering::Test>();
    entity.get<Components::Steering::Test>().speed = WALK_SPEED;

    entity.add<Components::ColoredRect>(RED, Vector2{PLAYER_SIZE, PLAYER_SIZE});
    entity.add<Tags::Renderable>();
}

void onEnterMultiplier(Events::Area::Enter event)
{
    auto &entity = *event.entity;
    auto &area = *event.area;

    if (entity.has<Components::Locomotion::Velocity>() && area.has<Components::Locomotion::Multiplier>())
    {
        auto &locomotion = entity.get<Components::Locomotion::Velocity>();
        auto &mult = area.get<Components::Locomotion::Multiplier>();

        locomotion.multiplier *= mult.multiplier;
    }
}

void onExitMultiplier(Events::Area::Exit event)
{
    auto &entity = *event.entity;
    auto &area = *event.area;

    if (entity.has<Components::Locomotion::Velocity>() && area.has<Components::Locomotion::Multiplier>())
    {
        auto &locomotion = entity.get<Components::Locomotion::Velocity>();
        auto &mult = area.get<Components::Locomotion::Multiplier>();

        locomotion.multiplier /= mult.multiplier;
    }
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    Scene scene;

    scene.onConstruct<Components::Locomotion::Multiplier>([](auto &entity) {
        if (entity.template has<Components::Area>())
        {
            auto &area = entity.template get<Components::Area>();
            area.dispatcher.template sink<Events::Area::Enter>().template connect<&onEnterMultiplier>();
            area.dispatcher.template sink<Events::Area::Exit>().template connect<&onExitMultiplier>();
        }
    });

    std::vector<std::unique_ptr<Systems::System>> systems;
    {
        using namespace Systems;

        systems.push_back(std::make_unique<Steering::Player>());
        systems.push_back(std::make_unique<Steering::Test>());

        systems.push_back(std::make_unique<Locomotion::Velocity>());

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
