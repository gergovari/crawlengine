#pragma once

#include "spatial_hash_map.hpp"

class Scene;
class Entity;

class Collidables : public SpacePartitioner<Entity *>
{
  private:
    SpatialHashMap<Entity *> items;

    Rectangle enlarge(Rectangle rect);

  public:
    Collidables(Scene *scene);

    void insert(Entity *item, Rectangle rect) override;
    void update(Entity *item, Rectangle rect) override;
    void replace(Entity *item, Entity *t) override;
    void remove(Entity *item) override;
    SpatialCell<Entity *> nearby(Rectangle rect) override;
};
