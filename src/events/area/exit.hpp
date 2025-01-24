#pragma once

class Entity;

namespace Events
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
