#include <cstdio>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <unordered_set>

#include "components.hpp"
#include "entity.hpp"
#include "event.hpp"
#include "raylib.h"
#include "raylib_extended.hpp"
#include "raymath.h"
#include "renderables.hpp"
#include "scene.hpp"
#include "system.hpp"

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
