#pragma once

class Entity;

namespace Event
{
    namespace Area
    {
        struct Exit
        {
            Entity *entity;
            Entity *area;
        };
    }
}
