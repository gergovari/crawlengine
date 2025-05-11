#include <cstdio>
#include <unordered_set>

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

#include "misc/raylib_extended.hpp"

#include "entity/entity.hpp"
#include "scene/renderables.hpp"
#include "scene/scene.hpp"


#include "systems/system.hpp"
#include "systems/service_wrapper.hpp"

#include "systems/locomotion/force.hpp"
#include "systems/locomotion/multiplier.hpp"

#include "systems/steering/player.hpp"
#include "systems/steering/test.hpp"

#include "systems/render_renderables.hpp"
#include "systems/follow_camera_targets.hpp"
#include "systems/area.hpp"


#include "services/inputs/raylib.hpp"
#include "services/renderers/raylib.hpp"


#include "components/colored_rect.hpp"
#include "components/camera.hpp"

#include "components/locomotion/multiplier.hpp"
#include "components/locomotion/force.hpp"
#include "components/locomotion/heading.hpp"

#include "components/steering/player.hpp"
#include "components/steering/test.hpp"

#include "components/tags/renderable.hpp"

/* TODO
 *
 * locomotion steering overhaul
 * steering behaviours
 *
 * resources system
 * sprint with stamina
 *
 * ai behaviours
 * fsm
 *
 * enemy attack player
 * allow player to hit back
 *
 * sustained effects
 * traps
 *
 * replace ColoredRects with sprites
 * Tiled maps
 *
 * ui
 * inventory system
 * equipment system
 * equipment effects 
 * 
 * audio
 *
 * travel between maps
 *
 * utility ai
 * dialog system
 * LLM
 * levels of sim
 *
 * migrate to raylib-cpp
 *
 * Scene combine .each
 *
 * shaders
 * post-processing
 *
 * SD
 */

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
    entity.add<Components::Locomotion::Force>();
    entity.add<Components::Locomotion::Heading>();
    entity.add<Components::Collider>(Vector2{PLAYER_SIZE, PLAYER_SIZE});
    entity.add<Components::Steering::Player>();

    entity.add<Components::ColoredRect>(GREEN, Vector2{PLAYER_SIZE, PLAYER_SIZE});
    entity.add<Tags::Renderable>();

    entity.add<Components::Camera>();
    Components::Camera &comp = entity.get<Components::Camera>();
    Camera2D *cam = &comp.cam;

    comp.target = &entity;
    comp.offset = raylib::Vector2(PLAYER_SIZE / 2, PLAYER_SIZE / 2);

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

    entity.add<Components::Locomotion::Force>();
    entity.add<Components::Locomotion::Heading>();

    entity.add<Components::Steering::Test>();
    entity.get<Components::Steering::Test>().speed = WALK_SPEED;

    entity.add<Components::ColoredRect>(RED, Vector2{PLAYER_SIZE, PLAYER_SIZE});
    entity.add<Tags::Renderable>();
}

static std::vector<std::unique_ptr<Systems::System>> SetupSystems(Scene &scene)
{
    using namespace Systems;

    std::vector<std::unique_ptr<Systems::System>> systems;

    entt::locator<Services::Inputs::Input>::emplace<Services::Inputs::Raylib>();
    systems.push_back(std::make_unique<ServiceWrapper<Services::Inputs::Input>>());

    systems.push_back(std::make_unique<Steering::Player>());
    systems.push_back(std::make_unique<Steering::Test>());

    systems.push_back(std::make_unique<Locomotion::Force>());
    systems.push_back(std::make_unique<Locomotion::Multiplier>(scene));

    systems.push_back(std::make_unique<Area>());

    systems.push_back(std::make_unique<FollowCameraTargets>());

    systems.push_back(std::make_unique<RenderRenderables>());

    entt::locator<Services::Renderers::Renderer>::emplace<Services::Renderers::Raylib>(1280, 720);
    systems.push_back(std::make_unique<ServiceWrapper<Services::Renderers::Renderer>>());

    return systems;
}

int main()
{
    Scene scene;
    auto systems = SetupSystems(scene);

    addWorld(scene);
    addPlayer(scene);
    addEnemy(scene);

    while (!WindowShouldClose())
    {
        for (auto &system : systems)
        {
            system->tick(scene);
        }
    }

    return 0;
}
