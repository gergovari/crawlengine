#pragma once

#include <raylib-cpp.hpp>

namespace Components
{
	namespace Locomotion
	{
		struct Force
		{
			float max = 1300;
			float mass = 90;

			raylib::Vector2 vel;
		};
	}
}
