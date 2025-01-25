#pragma once

#include <raylib-cpp.hpp>

namespace Components
{
    struct Transform
    {
        Vector2 pos = {0};
        operator Vector2() const
        {
            return pos;
        }
    };
}
