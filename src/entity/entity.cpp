#include "entity.hpp"

Entity::Entity(entt::registry *r) : registry(r)
{
    id = registry->create();
}

Entity::~Entity()
{
    registry->destroy(id);
}

Entity::operator EntityId() const
{
    return id;
}

bool Entity::operator==(const Entity &other) const
{
    return (registry == other.registry) && (id == other.id);
}
