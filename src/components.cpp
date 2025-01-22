#include "components.hpp"

namespace Component
{
    namespace Locomotion
    {
        void Velocity::setVelocity(Vector2 v)
        {
            vel = Vector2Clamp(v, {-targetSpeed, -targetSpeed}, {targetSpeed, targetSpeed});

            for (auto &modifier : modifiers)
            {
                vel = modifier(vel);
            }

            vel = Vector2Scale(vel, GetFrameTime());
        }
    }
}
