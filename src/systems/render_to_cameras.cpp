#include "misc/raylib_extended.hpp"

#include "systems/render_to_cameras.hpp"
#include "scene/scene.hpp"
#include "components/components.hpp"

namespace Systems {
    void RenderToCameras::tick(Scene &scene)
    {
        auto &renderables = scene.renderables;

        scene.each<Components::Camera>([&renderables](auto &comp) {
            auto &cam = comp.cam;

            BeginMode2D(cam);
            DrawRenderables(cam, renderables);
            EndMode2D();
        });
    }
}
