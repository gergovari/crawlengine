#pragma once

#include <deque>
#include <unordered_map>

#include <raylib-cpp.hpp>

#include "renderer.hpp"

#define WINDOW_TITLE "CRAWLengine"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

class Rectangle;

namespace Services
{
    namespace Renderers
    {
        class Raylib : public Renderer
        {
          private:
            std::unordered_map<raylib::Camera2D *, std::deque<ColoredShape<Rectangle>>> queues;
            raylib::Window window;

            void flush();
            void reset();

          public:
            Raylib(size_t w, size_t h);
            void draw(Renderers::ColoredShape<Rectangle> rect, raylib::Camera2D *cam = nullptr) override;
            void tick(Scene &scene) override;
        };
    }
}
