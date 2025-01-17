#include "entity.hpp"

Entity::Entity(entt::registry *r) : registry(r) {
	entity = registry->create();
}

Entity::~Entity() {
	registry->destroy(entity);
}

Entity::operator entt::entity() const {
	return entity;
}

bool Entity::operator==(const Entity &other) const {
	return (registry == other.registry) && (entity == other.entity);
}

