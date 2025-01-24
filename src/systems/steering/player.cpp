#include "systems/steering/player.hpp"

#include "scene.hpp"
#include "components/components.hpp"

namespace Systems {
    namespace Steering {
        void Player::tick(Scene &scene)
        {
            scene.each<Components::Steering::Player, Components::Locomotion::Velocity>(
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
}
