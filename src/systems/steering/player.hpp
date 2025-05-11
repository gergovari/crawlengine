#pragma once

#include "systems/system.hpp"

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
