#include "components.hpp"

namespace Component
{
    namespace Locomotion
    {
        void Velocity::setVelocity(Vector2 v)
        {
            vel = Vector2Clamp(v, {-targetSpeed, -targetSpeed}, {targetSpeed, targetSpeed});
            vel = Vector2Scale(vel, GetFrameTime());
        }
    }
}
