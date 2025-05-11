#pragma once

#include <raylib-cpp.hpp>

namespace AI {
	namespace Steering {
		class Behavior {
			public:
				virtual raylib::Vector2 calculate(raylib::Vector2 pos, raylib::Vector2 vel) = 0;
		};

		class Seek : public Behavior {
			public:
				raylib::Vector2 target;

				Seek(raylib::Vector2 t) : target(t) {};
				raylib::Vector2 calculate(raylib::Vector2 pos, raylib::Vector2 vel);
		};
	}
}
