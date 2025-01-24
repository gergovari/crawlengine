#pragma once

#include <raylib.h>

namespace Component
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
