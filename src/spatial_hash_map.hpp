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

template <typename T> class SpatialHashMap
{
  private:
    std::unordered_map<SpatialPair, SpatialCell<T>, SpatialPairHash> cells;
    const SpatialPair unit;

    void forEach(std::function<bool(SpatialCell<T> &)> func, Rectangle rect);
    void forEach(std::function<bool(SpatialCell<T> &)> func);

  public:
    explicit SpatialHashMap(SpatialPair u);

    void insert(T item, Rectangle rect);
    void update(T item, Rectangle rect);
    void replace(T item, T t);
    void remove(T item);
    SpatialCell<T> nearby(Rectangle rect);
};

#include "spatial_hash_map.tpp"
