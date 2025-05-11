#pragma once

#include "systems/system.hpp"

namespace Systems
{
	namespace Steering
	{
		class Test : public System
		{
			public:
				void tick(Scene &scene) override;
		};
	}
}
