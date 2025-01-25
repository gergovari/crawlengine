#pragma once

#include "systems/system.hpp"

#define WALK_SPEED 120.0f
#define SPRINT_SPEED WALK_SPEED * 2

namespace Systems
{
    namespace Steering
    {
        class Player : public System
        {
          public:
            void tick(Scene &scene) override;
        };
    }
}
