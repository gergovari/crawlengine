#pragma once

#include <raylib.h>

namespace Component
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
