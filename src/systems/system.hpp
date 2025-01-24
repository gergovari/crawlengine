#pragma once

class Scene;

namespace Systems
{
    class System
    {
      public:
        virtual void tick(Scene &scene) = 0;
        virtual ~System() = default;
    };
}
