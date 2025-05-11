#include "systems/render_renderables.hpp"

#include "misc/raylib_extended.hpp"
#include <entt/entt.hpp>

#include "scene/scene.hpp"
#include "services/renderers/renderer.hpp"

#include "components/camera.hpp"
#include "components/colored_rect.hpp"

namespace Systems
{
    void RenderRenderables::tick(Scene &scene)
    {
        if (entt::locator<Services::Renderers::Renderer>::has_value())
        {
            auto &renderer = entt::locator<Services::Renderers::Renderer>::value();
            auto &renderables = scene.renderables;

            scene.each<Components::Camera>([&renderer, &renderables](auto &comp) {
                auto &cam = comp.cam;
                auto view = GetCameraView(cam);
                auto items = renderables.nearby(view);

                items.sort([](auto const &a, auto const &b) { return a.z < b.z; });

                for (auto &renderable : items)
                {
                    Entity &entity = *renderable.item;
                    auto &transform = entity.get<Components::Transform>();

                    if (entity.has<Components::ColoredRect>())
                    {
                        auto &colored = entity.get<Components::ColoredRect>();
                        Rectangle rect = {transform.pos.x, transform.pos.y, colored.size.x, colored.size.y};
                        auto &color = colored.color;

                        renderer.draw({rect, color}, &cam);
                    }
                }
            });
        }
    }
}
