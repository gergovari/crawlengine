#pragma once

#include "components.hpp"
#include "entity.hpp"
#include "renderables.hpp"
#include "spatial_hash_map.hpp"
#include <entt/entt.hpp>
#include <forward_list>

typedef std::function<void(Entity &)> EntityCallback;
class SceneCallback;

class Scene
{
  private:
    entt::registry registry;

    static const RenderableItem<Entity *> eToRI(Entity &entity);
    static const RenderableItem<Entity *> eToRI(Entity &entity, int z);

  public:
    std::forward_list<Entity> entities;
    std::forward_list<SceneCallback> callbacks;
    SpatialHashMap<RenderableItem<Entity *>> renderables;
    SpatialHashMap<Entity *> colliders;

    Scene();

    void setupRenderables();
    void setupColliders();
    void setupTransforms();

    Entity &add();
    Entity *get(entt::entity e);
    void remove(Entity &entity);

    template <typename... ComponentTypes, typename Func> void eachE(Func &&func);
    template <typename... ComponentTypes, typename Func> void each(Func &&func);

    template <typename ComponentType> void onConstruct(EntityCallback func);
    template <typename ComponentType> void onUpdate(EntityCallback func);
    template <typename ComponentType> void onDestroy(EntityCallback func);
};

class SceneCallback
{
  private:
    Scene *scene;
    EntityCallback func;

  public:
    SceneCallback(Scene *s, EntityCallback f);

    void connect(entt::registry &registry, entt::entity e);
};

#include "scene.tpp"
