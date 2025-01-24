#pragma once

class Entity;

namespace Event
{
    namespace Area
    {
        struct Enter
        {
            Entity *entity;
            Entity *area;
        };
    }
}
