#pragma once

#include <raylib-cpp.hpp>

namespace Components
{
	namespace Locomotion
	{
		struct Heading
		{
			raylib::Vector2 dir;

			float maxVelocityScale = 1;
		};
	}
}
