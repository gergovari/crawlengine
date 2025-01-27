#include "systems/steering/player.hpp"

#include "components/components.hpp"
#include "scene/scene.hpp"

#include "services/services.hpp"

namespace Systems
{
    namespace Steering
    {
        void Player::tick(Scene &scene)
        {
            auto &input = entt::locator<Services::Inputs::Input>::value();

            scene.each<Components::Steering::Player, Components::Locomotion::Velocity>(
                [&input](const auto &steering, auto &locomotion) {
                    using namespace Services::Inputs;

                    if (input.isDown(MOVE_SPRINT))
                    {
                        locomotion.targetSpeed = SPRINT_SPEED;
                    }
                    else
                    {
                        locomotion.targetSpeed = WALK_SPEED;
                    }
                    Vector2 in = {(float)(input.isDown(MOVE_RIGHT) - input.isDown(MOVE_LEFT)),
                                  (float)(input.isDown(MOVE_DOWN) - input.isDown(MOVE_UP))};

                    locomotion.setVelocity(Vector2Scale(in, locomotion.targetSpeed));
                });
        }
    }
}
