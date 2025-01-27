#pragma once

#include <unordered_map>

#include <entt/entt.hpp>
#include <raylib-cpp.hpp>

#include "input.hpp"

namespace Services
{
    namespace Inputs
    {
        class Raylib : public Input
        {
            private:
                std::unordered_map<Type, KeyboardKey> keymap = {
                    { MOVE_UP, KEY_UP },
                    { MOVE_DOWN, KEY_DOWN },
                    { MOVE_LEFT, KEY_LEFT },
                    { MOVE_RIGHT, KEY_RIGHT },
                    { MOVE_SPRINT, KEY_LEFT_SHIFT }
                };

            public:
                entt::dispatcher dispatcher{};

                void tick(Scene &scene) override;
                bool isDown(Type type) override;
        };
    }
}
