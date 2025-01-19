#pragma once

#include "components.hpp"
#include "entity.hpp"
#include "renderables.hpp"
#include "spatial_hash_map.hpp"
#include <entt/entt.hpp>
#include <forward_list>

class Scene
{
  private:
    entt::registry registry;

    static const RenderableItem<Entity *> eToRI(Entity &entity);
    static const RenderableItem<Entity *> eToRI(Entity &entity, int z);

    void onDestroyRenderable(entt::registry &registry, entt::entity e);
    void onDestroyCollider(entt::registry &registry, entt::entity e);
    void onConstructRenderable(entt::registry &registry, entt::entity e);
    void onConstructCollider(entt::registry &registry, entt::entity e);
    void onUpdateTransformForRenderable(Entity &entity);
    void onUpdateTransformForCollider(Entity &entity);
    void onUpdateTransform(entt::registry &registry, entt::entity e);
    void onUpdateRenderable(entt::registry &registry, entt::entity e);

  public:
    std::forward_list<Entity> entities;
    SpatialHashMap<RenderableItem<Entity *>> renderables;
    SpatialHashMap<Entity *> colliders;

    Scene();

    void setupRenderables();
    void setupColliders();

    Entity &add();
    Entity *get(entt::entity e);
    void remove(Entity &entity);

    template <typename... ComponentTypes, typename Func> void eachE(Func &&func);
    template <typename... ComponentTypes, typename Func> void each(Func &&func);

    template <typename ComponentType, auto Func> void onConstruct();
    template <typename ComponentType, auto Func> void onUpdate();
    template <typename ComponentType, auto Func> void onDestroy();

    template <typename ComponentType, auto Func, typename Class> void onConstruct(Class inst);
    template <typename ComponentType, auto Func, typename Class> void onUpdate(Class inst);
    template <typename ComponentType, auto Func, typename Class> void onDestroy(Class inst);
};

#include "scene.tpp"
