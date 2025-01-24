#pragma once

class Entity;

namespace Events
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
