#pragma once

#include "entity/entity.hpp"
#include <raylib-cpp.hpp>

namespace Components
{
    struct Camera
    {
        raylib::Camera2D cam;
        Entity *target = nullptr;
        raylib::Vector2 offset = {0};
    };
}
