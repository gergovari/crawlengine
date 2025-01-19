#include "system.hpp"

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

    void Locomotion::tick(Scene &scene)
    {
        scene.eachE<Component::Transform, Component::Locomotion, Component::Collider>(
            [&scene](auto &entity, const auto &transform, const auto &locomotion, const auto &collider) {
                Rectangle collision;

                if (Vector2LengthSqr(locomotion.vel) != 0)
                {
                    entity.template update<Component::Transform>(
                        [&locomotion](auto &transform) { transform.pos.x += locomotion.vel.x; });

                    if (IsColliding(scene, TcToRect(transform, collider), collision))
                    {
                        entity.template update<Component::Transform>([&collision, &locomotion](auto &transform) {
                            transform.pos.x -= collision.width * ((locomotion.vel.x > 0) - (locomotion.vel.x < 0));
                        });
                    }

                    entity.template update<Component::Transform>(
                        [&locomotion](auto &transform) { transform.pos.y += locomotion.vel.y; });

                    if (IsColliding(scene, TcToRect(transform, collider), collision))
                    {
                        entity.template update<Component::Transform>([&collision, &locomotion](auto &transform) {
                            transform.pos.y -= collision.height * ((locomotion.vel.y > 0) - (locomotion.vel.y < 0));
                        });
                    }
                }
            });
    }

    namespace Steering
    {
        void Test::tick(Scene &scene)
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

        void Player::tick(Scene &scene)
        {
            scene.each<Component::Steering::Player, Component::Locomotion>([](const auto &input, auto &locomotion) {
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

    void Area::tick(Scene &scene)
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
                        const auto result = GetCollisionRec(TaToRect(transform, area), TcToRect(eTransform, eCollider));

                        if (result.width > 0 && result.height > 0)
                        {
                            area.dispatcher.template trigger<Event::Enter>(Event::Enter{entityP});
                        }
                    }
                }
            }
        });
    }
}
