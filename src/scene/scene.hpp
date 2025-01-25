#pragma once

#include "entity/entity.hpp"
#include "scene/collidables.hpp"
#include "scene/renderables.hpp"
#include <forward_list>

class SceneCallback;

typedef std::function<void(Entity &)> EntityCallback;

class Scene
{
  private:
    entt::registry registry;

    static const RenderableItem<Entity *> eToRI(Entity &entity);
    static const RenderableItem<Entity *> eToRI(Entity &entity, int z);

  public:
    std::forward_list<Entity> entities;
    std::forward_list<SceneCallback> callbacks;

    Renderables renderables;
    Collidables colliders;

    Scene();

    Entity &add();
    Entity *get(EntityId e);
    void remove(Entity &entity);

    template <typename... ComponentTypes, typename Func> void eachEntity(Func &&func);
    template <typename... ComponentTypes, typename Func> void eachId(Func &&func);
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
