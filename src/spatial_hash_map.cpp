#include "spatial_hash_map.hpp"

std::size_t SpatialPairHash::operator()(const SpatialPair &pair) const {
	size_t lhs = pair.first;
	const size_t rhs = pair.second;

	lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
	return lhs;
}
