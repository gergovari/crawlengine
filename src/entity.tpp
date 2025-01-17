#pragma once

template<typename Type, typename... Args>
decltype(auto) Entity::add(Args &&...args) {
	return registry->emplace<Type>(entity, std::forward<Args>(args)...);
}

template<typename Type, typename... Args>
decltype(auto) Entity::update(Args &&...args) {
	return registry->patch<Type>(entity, std::forward<Args>(args)...);
}

template<typename Type>
decltype(auto) Entity::get() const {
	return registry->get<Type>(entity);
}

template<typename... Args>
decltype(auto) Entity::has() const {
	return registry->all_of<Args...>(entity);
}

template<typename... Args>
decltype(auto) Entity::remove() {
	return registry->remove<Args...>(entity);
}
