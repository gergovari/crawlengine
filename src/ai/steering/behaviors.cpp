#include "behaviors.hpp"

using namespace raylib;

namespace AI {
	namespace Steering {
		raylib::Vector2 Seek::calculate(raylib::Vector2 pos, raylib::Vector2 vel) {
			raylib::Vector2 dir = (pos - target).Negate().Normalize();

			return dir;
		};
	}
}
