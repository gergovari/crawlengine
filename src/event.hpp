#pragma once

#include "components.hpp"
#include "entity.hpp"

namespace Event
{
    struct Enter
    {
        Entity *entity;
        Entity *area;
    };

    struct Exit
    {
        Entity *entity;
        Entity *area;
    };
}
