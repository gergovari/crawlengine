#pragma once

#include <raylib.h>

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
