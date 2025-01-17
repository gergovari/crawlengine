#pragma once

#include <entt/entt.hpp>

class Entity
{
  private:
    entt::registry *registry;
    entt::entity entity;

  public:
    explicit Entity(entt::registry *r);
    ~Entity();

    operator entt::entity() const;

    bool operator==(const Entity &other) const;

    template <typename Type, typename... Args> decltype(auto) add(Args &&...args);

    template <typename Type, typename... Args> decltype(auto) update(Args &&...args);

    template <typename Type> decltype(auto) get() const;

    template <typename... Args> decltype(auto) has() const;

    template <typename... Args> decltype(auto) remove();
};

#include "entity.tpp"
