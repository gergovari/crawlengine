#include "systems/locomotion/multiplier.hpp"

#include "components/components.hpp"
#include "scene/scene.hpp"
#include "events/events.hpp"

namespace Systems {
    namespace Locomotion {
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

        Multiplier::Multiplier(Scene &scene) {
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
