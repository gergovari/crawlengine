#pragma once

class Entity;

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
