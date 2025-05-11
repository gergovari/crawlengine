#pragma once

#include "systems/system.hpp"

namespace Systems
{
    namespace Locomotion
    {
        class Force : public System
        {
          public:
            void tick(Scene &scene) override;
        };
    }
}
