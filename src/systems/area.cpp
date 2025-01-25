#include "systems/area.hpp"

#include "components/components.hpp"
#include "entity/entity.hpp"
#include "events/events.hpp"
#include "misc/raylib_extended.hpp"
#include "scene/scene.hpp"

namespace Systems
{
    const Rectangle Area::CreateSearchRect(const Components::Transform &transform, const Components::Area &area)
    {
        return {transform.pos.x - area.size.x, transform.pos.y - area.size.y, area.size.x * 3, area.size.y * 3};
    }

    void Area::tick(Scene &scene)
    {
        scene.eachId<Components::Area, Components::Transform>(
            [this, &scene](auto id, auto &area, const auto &transform) {
                auto entities = scene.colliders.nearby(CreateSearchRect(transform, area));
                auto &counters = areas[area.id];
                std::vector<Entity *> current;

                for (auto &p : entities)
                {
                    auto &entity = *p;

                    if (entity.template has<Components::Transform, Components::Collider>())
                    {
                        const auto collision = GetCollisionRec(TaToRect(transform, area),
                                                               TcToRect(entity.template get<Components::Transform>(),
                                                                        entity.template get<Components::Collider>()));

                        if (IsRectValid(collision))
                        {
                            current.push_back(p);
                            if (std::find(area.current.begin(), area.current.end(), p) == area.current.end())
                            {
                                auto it = std::find_if(counters.begin(), counters.end(),
                                                       [&p](const auto &counted) { return p == counted.entity; });

                                if (it == counters.end())
                                {
                                    area.dispatcher.template trigger(Events::Area::Enter{p, scene.get(id)});
                                    counters.emplace_back(p, 1);
                                }
                                else
                                {
                                    it->count++;
                                }
                            }
                        }
                    }
                }

                for (auto counted = counters.begin(); counted != counters.end();)
                {
                    bool isHere = std::find(current.begin(), current.end(), counted->entity) != current.end();
                    bool wasHere =
                        std::find(area.current.begin(), area.current.end(), counted->entity) != area.current.end();
                    if (wasHere && !isHere)
                    {
                        if (--counted->count <= 0)
                        {
                            area.dispatcher.template trigger(Events::Area::Exit{counted->entity, scene.get(id)});
                            counted = counters.erase(counted);
                            continue;
                        }
                    }
                    ++counted;
                }

                area.current = current;
            });
    }
}
