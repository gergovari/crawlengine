#pragma once

#include <cmath>
#include <forward_list>
#include <functional>
#include <unordered_map>

#include "raylib.h"

typedef std::pair<int, int> SpatialPair;

template <typename T> using SpatialCell = typename std::forward_list<T>;

struct SpatialPairHash
{
    std::size_t operator()(const SpatialPair &pair) const;
};

template <typename T> class SpacePartitioner
{
  public:
    virtual void insert(T item, Rectangle rect) = 0;
    virtual void update(T item, Rectangle rect) = 0;
    virtual void replace(T item, T t) = 0;
    virtual void remove(T item) = 0;
    virtual SpatialCell<T> nearby(Rectangle rect) = 0;
};

template <typename T> class SpatialHashMap : public SpacePartitioner<T>
{
  private:
    std::unordered_map<SpatialPair, SpatialCell<T>, SpatialPairHash> cells;
    const SpatialPair unit;

    void forEach(std::function<bool(SpatialCell<T> &)> func, Rectangle rect);
    void forEach(std::function<bool(SpatialCell<T> &)> func);

  public:
    SpatialHashMap(SpatialPair u);

    void insert(T item, Rectangle rect);
    void update(T item, Rectangle rect);
    void replace(T item, T t);
    void remove(T item);
    SpatialCell<T> nearby(Rectangle rect);
};

#include "spatial_hash_map.tpp"
