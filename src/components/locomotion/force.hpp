#pragma once

#include <raylib-cpp.hpp>

namespace Components
{
	namespace Locomotion
	{
		struct Force
		{
			float maxForce = 1000000;
			float maxVelocity = 200;
			float mass = 90;

			raylib::Vector2 vel;
		};
	}
}
