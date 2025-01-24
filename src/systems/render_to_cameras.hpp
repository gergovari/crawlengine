#pragma once

#include "systems/system.hpp"

namespace Systems {
    class RenderToCameras : public System
    {
      public:
        void tick(Scene &scene) override;
    };
}
