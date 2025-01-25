#pragma once

#include "systems/system.hpp"

namespace Systems
{
    template <typename Service> class ServiceWrapper : public System
    {
        static_assert(std::is_base_of<System, Service>::value, "Service must also derive from System to be wrapped.");

      public:
        void tick(Scene &scene) override
        {
            if (entt::locator<Service>::has_value())
            {
                auto &service = entt::locator<Service>::value();

                service.tick(scene);
            }
        }
    };
}
