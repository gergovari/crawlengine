#pragma once

#include "components/components.hpp"
#include "entity.hpp"
#include "raylib.h"
#include "raylib_extended.hpp"
#include "scene.hpp"
#include <unordered_map>
#include <unordered_set>

#define WALK_SPEED 120.0f
#define SPRINT_SPEED WALK_SPEED * 2

namespace Systems
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
      private:
        struct CountedArea
        {
            Entity *entity = nullptr;
            size_t count = 0;
        };

        std::unordered_map<size_t, std::vector<CountedArea>> areas;

        const static Rectangle CreateSearchRect(const Components::Transform &transform, const Components::Area &area);

      public:
        void tick(Scene &scene) override;
    };
}
