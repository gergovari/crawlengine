#pragma once

#include "systems/system.hpp"

namespace Systems
{
    class RenderRenderables : public System
    {
      public:
        void tick(Scene &scene) override;
    };
}
