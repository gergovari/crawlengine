#include "systems/follow_camera_targets.hpp"

#include "components/components.hpp"
#include "scene/scene.hpp"

namespace Systems
{
    void FollowCameraTargets::tick(Scene &scene)
    {
        scene.each<Components::Camera>([](auto &comp) {
            auto &cam = comp.cam;
            auto *entityP = comp.target;

            if (entityP)
            {
                auto &entity = *entityP;

                if (entity.template has<Components::Transform>())
                {
                    cam.target = Vector2Add(entity.template get<Components::Transform>(), comp.offset);
                }
            }
        });
    }
}
