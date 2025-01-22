#pragma once

#include "spatial_hash_map.hpp"

class Scene;
class Entity;

template <typename T> struct RenderableItem
{
    T item;
    int z;

    bool operator==(const RenderableItem<T> &other) const;
};

class Renderables : public SpacePartitioner<RenderableItem<Entity *>>
{
  private:
    SpatialHashMap<RenderableItem<Entity *>> items;

    const RenderableItem<Entity *> eToRI(Entity &entity, int z);
    const RenderableItem<Entity *> eToRI(Entity &entity);
    Rectangle enlarge(Rectangle rect);

  public:
    Renderables(Scene *scene);

    void insert(RenderableItem<Entity *> item, Rectangle rect) override;
    void update(RenderableItem<Entity *> item, Rectangle rect) override;
    void replace(RenderableItem<Entity *> item, RenderableItem<Entity *> t) override;
    void remove(RenderableItem<Entity *> item) override;
    SpatialCell<RenderableItem<Entity *>> nearby(Rectangle rect) override;
};

#include "renderables.tpp"
