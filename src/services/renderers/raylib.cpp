#include "raylib.hpp"

#include <raylib-cpp.hpp>

/* TODO: customizable window title */
Services::Renderers::Raylib::Raylib(size_t w, size_t h) : window(w, h, "crawlEngine")
{
	SetWindowState(FLAG_VSYNC_HINT);
}

void Services::Renderers::Raylib::draw(Services::Renderers::ColoredShape<Rectangle> rect, raylib::Camera2D *cam)
{
    queues[cam].push_front(rect);
}

void Services::Renderers::Raylib::flush()
{
    for (auto &[cam, items] : queues)
    {
        if (cam)
        {
            cam->BeginMode();
        }

        while (!items.empty())
        {
            auto &item = items.back();

            item.color.DrawRectangle(item.shape);

            items.pop_back();
        }

        if (cam)
        {
            cam->EndMode();
        }
    }
}

void Services::Renderers::Raylib::reset()
{
    window.ClearBackground(RAYWHITE);
}

void Services::Renderers::Raylib::tick(Scene &scene)
{
    BeginDrawing();
    reset();

    flush();

    DrawFPS(0, 0);
    EndDrawing();
}
