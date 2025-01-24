#pragma once

#include "entity/entity.hpp"
#include <entt/entt.hpp>
#include <raylib.h>

namespace Components
{
    struct Area
    {
        size_t id = 0;
        Vector2 size = {0};

        std::vector<Entity *> current;
        entt::dispatcher dispatcher{};
    };
}
