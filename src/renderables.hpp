#pragma once

template <typename T> struct RenderableItem
{
    T item;
    int z;

    bool operator==(const RenderableItem<T> &other) const;
};

#include "renderables.tpp"
