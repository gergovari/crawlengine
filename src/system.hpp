#pragma once

#include "components.hpp"
#include "event.hpp"
#include "raylib.h"
#include "raylib_extended.hpp"
#include "scene.hpp"

#define WALK_SPEED 120.0f
#define SPRINT_SPEED WALK_SPEED * 2

namespace System
{
    class System
    {
      public:
        virtual void tick(Scene &scene) = 0;
        virtual ~System() = default;
    };

    class RenderToCameras : public System
    {
      public:
        void tick(Scene &scene) override;
    };

    class FollowCameraTargets : public System
    {
      public:
        void tick(Scene &scene) override;
    };
    
    namespace Locomotion
    {
        class Velocity : public System
        {
          public:
            void tick(Scene &scene) override;
        };
    }

    namespace Steering
    {
        class Test : public System
        {
          public:
            void tick(Scene &scene) override;
        };

        class Player : public System
        {
          public:
            void tick(Scene &scene) override;
        };
    }

    class Area : public System
    {
      public:
        void tick(Scene &scene) override;
    };
}
