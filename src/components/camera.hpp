#pragma once

#include "entity/entity.hpp"
#include <raylib.h>

namespace Components
{
    struct Camera
    {
        Camera2D cam = {0};
        Entity *target = nullptr;
        Vector2 offset = {0};
    };
}
