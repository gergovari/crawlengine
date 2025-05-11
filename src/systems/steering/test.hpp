#pragma once

#include "systems/system.hpp"

#include "ai/steering/behaviors.hpp"

namespace Systems
{
	namespace Steering
	{
		class Test : public System
		{
			private:
				AI::Steering::Seek seek;
			public:
				Test() : seek(raylib::Vector2(125, 500)) {};
				void tick(Scene &scene) override;
		};
	}
}
