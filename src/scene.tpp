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

template<typename ComponentType, auto Func> void Scene::onConstruct()
{
    registry.on_construct<ComponentType>().template connect<Func>(this);
}

template<typename ComponentType, auto Func> void Scene::onUpdate()
{
    registry.on_update<ComponentType>().template connect<Func>(this);
}

template<typename ComponentType, auto Func> void Scene::onDestroy()
{
    registry.on_destroy<ComponentType>().template connect<Func>(this);
}

template<typename ComponentType, auto Func, typename Class> void Scene::onConstruct(Class inst)
{
    registry.on_construct<ComponentType>().template connect<Func>(inst);
}

template<typename ComponentType, auto Func, typename Class> void Scene::onUpdate(Class inst)
{
    registry.on_update<ComponentType>().template connect<Func>(inst);
}

template<typename ComponentType, auto Func, typename Class> void Scene::onDestroy(Class inst)
{
    registry.on_destroy<ComponentType>().template connect<Func>(inst);
}
