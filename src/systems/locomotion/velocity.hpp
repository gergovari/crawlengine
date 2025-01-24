#pragma once

#include "systems/system.hpp"

namespace Systems {
    namespace Locomotion
    {
        class Velocity : public System
        {
          public:
            void tick(Scene &scene) override;
        };
    }
}
