#pragma once

#include <raylib-cpp.hpp>

namespace Components
{
	namespace Locomotion
	{
		struct Force
		{
			float max = 100;
			float mass = 1;

			raylib::Vector2 vel;
		};
	}
}
