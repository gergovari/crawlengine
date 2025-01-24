#include "systems/locomotion/velocity.hpp"

#include "raylib_extended.hpp"
#include "scene.hpp"
#include "entity.hpp"

namespace Systems {
    namespace Locomotion
    {
        void Velocity::tick(Scene &scene)
        {
            scene.eachEntity<Components::Transform, Components::Locomotion::Velocity, Components::Collider>(
                [&scene](auto &entity, const auto &transform, const auto &locomotion, const auto &collider) {
                    Rectangle collision;

                    if (Vector2LengthSqr(locomotion.vel) != 0)
                    {
                        entity.template update<Components::Transform>([&locomotion](auto &transform) {
                            transform.pos.x += locomotion.vel.x * locomotion.multiplier;
                        });

                        if (IsColliding(scene, TcToRect(transform, collider), collision))
                        {
                            entity.template update<Components::Transform>([&collision, &locomotion](auto &transform) {
                                transform.pos.x -= collision.width * ((locomotion.vel.x > 0) - (locomotion.vel.x < 0));
                            });
                        }

                        entity.template update<Components::Transform>([&locomotion](auto &transform) {
                            transform.pos.y += locomotion.vel.y * locomotion.multiplier;
                        });

                        if (IsColliding(scene, TcToRect(transform, collider), collision))
                        {
                            entity.template update<Components::Transform>([&collision, &locomotion](auto &transform) {
                                transform.pos.y -= collision.height * ((locomotion.vel.y > 0) - (locomotion.vel.y < 0));
                            });
                        }
                    }
                });
        }
    }
}
