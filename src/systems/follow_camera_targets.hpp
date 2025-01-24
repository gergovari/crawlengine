#pragma once

#include "systems/system.hpp"

namespace Systems {
    class FollowCameraTargets : public System
    {
      public:
        void tick(Scene &scene) override;
    };
}
