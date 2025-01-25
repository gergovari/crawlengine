#pragma once

#include <raylib-cpp.hpp>

namespace Components
{
    namespace Locomotion
    {
        struct Velocity
        {
            Vector2 vel = {0};
            float targetSpeed = 0;
            float multiplier = 1;

            void setVelocity(Vector2 v);
        };
    }
}
