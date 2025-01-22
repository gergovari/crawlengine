#pragma once

#include "entity.hpp"

namespace Event
{
    struct Enter
    {
        Entity *entity;
    };

    struct Exit
    {
        Entity *entity;
    };
}
