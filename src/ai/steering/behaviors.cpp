#include "behaviors.hpp"

using namespace raylib;

namespace AI {
	namespace Steering {
		raylib::Vector2 Behavior::seek(raylib::Vector2 pos, raylib::Vector2 target, float radius) {
			const auto diff = target - pos;
			if (diff.LengthSqr() <= radius*radius || radius == 0) {
				return (target - pos).Normalize();
			}
			return raylib::Vector2();
		};

		raylib::Vector2 Behavior::flee(raylib::Vector2 pos, raylib::Vector2 target, float radius) {
			return seek(pos, target, radius).Negate();
		};

		raylib::Vector2 Behavior::arrive(raylib::Vector2 pos, raylib::Vector2 target) {
			const auto rate = .1;
			const auto diff = target - pos;
			const auto scale = diff.Length() / 100;

			return diff.Normalize().Scale(scale);
		};
	}
}
