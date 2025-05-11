#include "systems/locomotion/multiplier.hpp"

#include "scene/scene.hpp"

#include "events/area/enter.hpp"
#include "events/area/exit.hpp"

#include "components/area.hpp"

#include "components/locomotion/velocity.hpp"
#include "components/locomotion/multiplier.hpp"

namespace Systems
{
    namespace Locomotion
    {
        void Multiplier::OnEnter(Events::Area::Enter event)
        {
            auto &entity = *event.entity;
            auto &area = *event.area;

            if (entity.has<Components::Locomotion::Velocity>() && area.has<Components::Locomotion::Multiplier>())
            {
                auto &locomotion = entity.get<Components::Locomotion::Velocity>();
                auto &mult = area.get<Components::Locomotion::Multiplier>();

                locomotion.multiplier *= mult.multiplier;
            }
        }

        void Multiplier::OnExit(Events::Area::Exit event)
        {
            auto &entity = *event.entity;
            auto &area = *event.area;

            if (entity.has<Components::Locomotion::Velocity>() && area.has<Components::Locomotion::Multiplier>())
            {
                auto &locomotion = entity.get<Components::Locomotion::Velocity>();
                auto &mult = area.get<Components::Locomotion::Multiplier>();

                locomotion.multiplier /= mult.multiplier;
            }
        }

        Multiplier::Multiplier(Scene &scene)
        {
            scene.onConstruct<Components::Locomotion::Multiplier>([](auto &entity) {
                if (entity.template has<Components::Area>())
                {
                    auto &area = entity.template get<Components::Area>();
                    area.dispatcher.template sink<Events::Area::Enter>().template connect<&Multiplier::OnEnter>();
                    area.dispatcher.template sink<Events::Area::Exit>().template connect<&Multiplier::OnExit>();
                }
            });
        }

        void Multiplier::tick(Scene &scene) {};
    }
}
