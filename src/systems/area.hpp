#pragma once

#include "systems/system.hpp"
#include "components/components.hpp"
#include <unordered_map>
#include <vector>

class Entity;
class Rectangle;

namespace Systems {
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
