#include "systems/steering/test.hpp"

#include "scene/scene.hpp"
#include "components/components.hpp"

namespace Systems
{
    namespace Steering
    {
        void Test::tick(Scene &scene)
        {
            scene.each<Components::Steering::Test, Components::Locomotion::Velocity>(
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

    }

}
