#pragma once

#include <raylib-cpp.hpp>

namespace Components
{
	namespace Locomotion
	{
		struct Velocity
		{
			float speed = 0;

			float multiplier = 1;
		};
	}
}
