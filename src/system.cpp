#include "system.hpp"

#include "events/events.hpp"
#include "raylib_extended.hpp"

namespace System
{
    void RenderToCameras::tick(Scene &scene)
    {
        auto &renderables = scene.renderables;

        scene.each<Component::Camera>([&renderables](auto &comp) {
            auto &cam = comp.cam;

            BeginMode2D(cam);
            DrawRenderables(cam, renderables);
            EndMode2D();
        });
    }

    void FollowCameraTargets::tick(Scene &scene)
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

    namespace Locomotion
    {
        void Velocity::tick(Scene &scene)
        {
            scene.eachEntity<Component::Transform, Component::Locomotion::Velocity, Component::Collider>(
                [&scene](auto &entity, const auto &transform, const auto &locomotion, const auto &collider) {
                    Rectangle collision;

                    if (Vector2LengthSqr(locomotion.vel) != 0)
                    {
                        entity.template update<Component::Transform>([&locomotion](auto &transform) {
                            transform.pos.x += locomotion.vel.x * locomotion.multiplier;
                        });

                        if (IsColliding(scene, TcToRect(transform, collider), collision))
                        {
                            entity.template update<Component::Transform>([&collision, &locomotion](auto &transform) {
                                transform.pos.x -= collision.width * ((locomotion.vel.x > 0) - (locomotion.vel.x < 0));
                            });
                        }

                        entity.template update<Component::Transform>([&locomotion](auto &transform) {
                            transform.pos.y += locomotion.vel.y * locomotion.multiplier;
                        });

                        if (IsColliding(scene, TcToRect(transform, collider), collision))
                        {
                            entity.template update<Component::Transform>([&collision, &locomotion](auto &transform) {
                                transform.pos.y -= collision.height * ((locomotion.vel.y > 0) - (locomotion.vel.y < 0));
                            });
                        }
                    }
                });
        }
    }

    namespace Steering
    {
        void Test::tick(Scene &scene)
        {
            scene.each<Component::Steering::Test, Component::Locomotion::Velocity>(
                [](const auto &test, auto &locomotion) {
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

        void Player::tick(Scene &scene)
        {
            scene.each<Component::Steering::Player, Component::Locomotion::Velocity>(
                [](const auto &input, auto &locomotion) {
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
    }

    const Rectangle Area::CreateSearchRect(const Component::Transform &transform, const Component::Area &area)
    {
        return {transform.pos.x - area.size.x, transform.pos.y - area.size.y, area.size.x * 3, area.size.y * 3};
    }

    void Area::tick(Scene &scene)
    {
        scene.eachId<Component::Area, Component::Transform>([this, &scene](auto id, auto &area, const auto &transform) {
            auto entities = scene.colliders.nearby(CreateSearchRect(transform, area));
            auto &counters = areas[area.id];
            std::vector<Entity *> current;

            for (auto &p : entities)
            {
                auto &entity = *p;

                if (entity.template has<Component::Transform, Component::Collider>())
                {
                    const auto collision = GetCollisionRec(TaToRect(transform, area),
                                                           TcToRect(entity.template get<Component::Transform>(),
                                                                    entity.template get<Component::Collider>()));

                    if (IsRectValid(collision))
                    {
                        current.push_back(p);
                        if (std::find(area.current.begin(), area.current.end(), p) == area.current.end())
                        {
                            auto it = std::find_if(counters.begin(), counters.end(),
                                                   [&p](const auto &counted) { return p == counted.entity; });

                            if (it == counters.end())
                            {
                                area.dispatcher.template trigger(Event::Area::Enter{p, scene.get(id)});
                                counters.emplace_back(p, 1);
                            }
                            else
                            {
                                it->count++;
                            }
                        }
                    }
                }
            }

            for (auto counted = counters.begin(); counted != counters.end();)
            {
                bool isHere = std::find(current.begin(), current.end(), counted->entity) != current.end();
                bool wasHere =
                    std::find(area.current.begin(), area.current.end(), counted->entity) != area.current.end();
                if (wasHere && !isHere)
                {
                    if (--counted->count <= 0)
                    {
                        area.dispatcher.template trigger(Event::Area::Exit{counted->entity, scene.get(id)});
                        counted = counters.erase(counted);
                        continue;
                    }
                }
                ++counted;
            }

            area.current = current;
        });
    }
}
