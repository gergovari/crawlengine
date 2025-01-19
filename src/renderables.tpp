#pragma once

template <typename T>
bool RenderableItem<T>::operator==(const RenderableItem<T> &other) const
{
    return item == other.item;
}
