#pragma once

#include "systems/system.hpp"
#include "events/events.hpp"

namespace Systems {
    namespace Locomotion {
        class Multiplier : public System {
            private:
                static void OnEnter(Events::Area::Enter event);
                static void OnExit(Events::Area::Exit event);

            public:
                Multiplier(Scene &scene);
                void tick(Scene &scene) override;
        };
    }
}
