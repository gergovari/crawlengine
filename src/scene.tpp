#pragma once

#include "scene.hpp"

template <typename... ComponentTypes, typename Func> void Scene::eachE(Func &&func)
{
    registry.view<ComponentTypes...>().each([this, func = std::forward<Func>(func)](auto e, auto &...components) {
        auto *entityP = get(e);
        if (entityP)
        {
            func(*entityP, components...);
        }
    });
}

template <typename... ComponentTypes, typename Func> void Scene::each(Func &&func)
{
    registry.view<ComponentTypes...>().each(
        [func = std::forward<Func>(func)](auto, auto &...components) { func(components...); });
}
