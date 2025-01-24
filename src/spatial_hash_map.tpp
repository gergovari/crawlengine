#pragma once

#include "raylib.h"
#include <cmath>

#include "spatial_hash_map.hpp"

template <typename T> SpatialHashMap<T>::SpatialHashMap(SpatialPair u) : unit(u)
{
}

template <typename T> void SpatialHashMap<T>::forEach(std::function<bool(SpatialCell<T> &)> func, Rectangle rect)
{
    auto steps = std::make_pair((int)std::floor(rect.width / unit.first), (int)std::floor(rect.height / unit.second));
    auto base = std::make_pair((int)std::floor(rect.x / unit.first), (int)std::floor(rect.y / unit.second));

    for (int i = 0; i < steps.first; i++)
    {
        for (int j = 0; j < steps.second; j++)
        {
            const auto pos = std::make_pair(base.first + i, base.second + j);

            if (func(cells[pos]))
            {
                return;
            }
        }
    }
}

template <typename T> void SpatialHashMap<T>::forEach(std::function<bool(SpatialCell<T> &)> func)
{
    for (auto &[pos, cell] : cells)
    {
        if (func(cell))
        {
            return;
        }
    }
}

template <typename T> void SpatialHashMap<T>::insert(T item, Rectangle rect)
{
    forEach(
        [&item, &rect](auto &cell) {
            cell.emplace_front(item);
            return false;
        },
        rect);
}

template <typename T> void SpatialHashMap<T>::update(T item, Rectangle rect)
{
    remove(item);
    insert(item, rect);
}

template <typename T> void SpatialHashMap<T>::replace(T item, T t)
{
    forEach([&item, &t](auto &cell) {
        for (auto &current : cell)
        {
            if (current == item)
            {
                current = t;
                return true;
            }
        }
        return false;
    });
}

template <typename T> void SpatialHashMap<T>::remove(T item)
{
    forEach([&item](auto &cell) {
        auto prev = cell.before_begin();

        for (auto it = cell.begin(); it != cell.end(); ++it)
        {
            if (*it == item)
            {
                it = cell.erase_after(prev);
                return true;
            }

            prev = it;
        }
        return false;
    });
}

template <typename T> SpatialCell<T> SpatialHashMap<T>::nearby(Rectangle rect)
{
    SpatialCell<T> result;

    forEach(
        [&result](auto &cell) {
            for (auto &item : cell)
            {
                result.push_front(item);
            }
            return false;
        },
        rect);

    return result;
}
