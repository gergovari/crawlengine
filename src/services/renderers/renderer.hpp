#pragma once

/* TODO: have our own "primitives" so we don't depend so much on raylib */
#include <raylib-cpp.hpp>

#include "systems/system.hpp"

class Scene;

namespace Services
{
    namespace Renderers
    {
        template <typename Shape> struct ColoredShape
        {
            Shape shape;
            raylib::Color color;
        };

        class Renderer : public Systems::System
        {
          public:
            virtual void draw(ColoredShape<Rectangle> rect, raylib::Camera2D *cam = nullptr) = 0;
            virtual void tick(Scene &scene) = 0;
        };
    }
}
