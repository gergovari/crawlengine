#pragma once

#include <raylib-cpp.hpp>

namespace AI {
	namespace Steering {
		class Behavior {
			public:
				static raylib::Vector2 seek(raylib::Vector2 pos, raylib::Vector2 target, float radius = 0);
				static raylib::Vector2 flee(raylib::Vector2 pos, raylib::Vector2 target, float radius = 0);
				static raylib::Vector2 arrive(raylib::Vector2 pos, raylib::Vector2 target);
		};
	}
}
