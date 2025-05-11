#include "systems/steering/test.hpp"

#include "scene/scene.hpp"

#include "components/transform.hpp"

#include "components/steering/test.hpp"
#include "components/locomotion/heading.hpp"

namespace Systems
{
	namespace Steering
	{
		void Test::tick(Scene &scene)
		{
			scene.eachEntity<Components::Steering::Test, Components::Locomotion::Heading, Components::Transform>(
						[this](auto &entity, const auto &test, auto &heading, const auto &transform) {
						Vector2 in = seek.calculate(transform.pos, heading.dir);

						entity.template update<Components::Locomotion::Heading>([&in](auto &heading) {
							heading.dir = in;
						});
			});
		}

	}

}
