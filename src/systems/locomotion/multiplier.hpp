#pragma once

#include "events/area/enter.hpp"
#include "events/area/exit.hpp"

#include "systems/system.hpp"

namespace Systems
{
    namespace Locomotion
    {
        class Multiplier : public System
        {
          private:
            static void OnEnter(Events::Area::Enter event);
            static void OnExit(Events::Area::Exit event);

          public:
            Multiplier(Scene &scene);
            void tick(Scene &scene) override;
        };
    }
}
