#pragma once

/* TODO: have our own "primitives" so we don't depend so much on raylib */
#include <raylib-cpp.hpp>

#include "systems/system.hpp"

class Scene;

namespace Services
{
    namespace Inputs
    {
        enum Type {
            MOVE_UP,
            MOVE_DOWN,
            MOVE_LEFT,
            MOVE_RIGHT,
            MOVE_SPRINT
        };

        class Input : public Systems::System
        {
          public:
            virtual void tick(Scene &scene) = 0;
            
            virtual bool isDown(Type type) = 0;
        };
    }
}
