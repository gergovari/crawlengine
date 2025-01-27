#include "raylib.hpp"

#include <raylib-cpp.hpp>

#include "events/inputs/input.hpp"

void Services::Inputs::Raylib::tick(Scene &scene)
{
    for (auto &[key, value] : keymap) {
        if (IsKeyDown(value)) {
            dispatcher.enqueue<Events::Inputs::Input>(key);
        }
    }
    dispatcher.update<Events::Inputs::Input>();
}

bool Services::Inputs::Raylib::isDown(Type type)
{
    return raylib::Keyboard::IsKeyDown(keymap[type]);
}
