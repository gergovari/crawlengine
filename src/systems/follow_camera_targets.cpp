#include "systems/follow_camera_targets.hpp"

#include "scene/scene.hpp"

#include "components/camera.hpp"
#include "components/transform.hpp"

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
