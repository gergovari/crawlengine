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

template <typename ComponentType> void Scene::onConstruct(EntityCallback func)
{
    registry.on_construct<ComponentType>().template connect<&SceneCallback::connect>(
        callbacks.emplace_front(this, func));
}

template <typename ComponentType> void Scene::onUpdate(EntityCallback func)
{
    registry.on_update<ComponentType>().template connect<&SceneCallback::connect>(callbacks.emplace_front(this, func));
}

template <typename ComponentType> void Scene::onDestroy(EntityCallback func)
{
    registry.on_destroy<ComponentType>().template connect<&SceneCallback::connect>(callbacks.emplace_front(this, func));
}
