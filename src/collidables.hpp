#pragma once

#include "space_partitioners/spatial_hash_map.hpp"

class Scene;
class Entity;
class Rectangle;

class Collidables : public SpacePartitioners::SpacePartitioner<Entity *>
{
    private:
        SpacePartitioners::SpatialHashMap<Entity *> items;

        Rectangle enlarge(Rectangle rect);

    public:
        Collidables(Scene *scene);

        void insert(Entity *item, Rectangle rect) override;
        void update(Entity *item, Rectangle rect) override;
        void replace(Entity *item, Entity *t) override;
        void remove(Entity *item) override;
        SpacePartitioners::SpatialCell<Entity *> nearby(Rectangle rect) override;
};
