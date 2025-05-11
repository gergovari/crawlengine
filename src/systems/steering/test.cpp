#include "systems/steering/test.hpp"

#include "scene/scene.hpp"

#include "components/transform.hpp"

#include "components/steering/test.hpp"
#include "components/locomotion/heading.hpp"

#include "ai/steering/behaviors.hpp"

namespace Systems
{
	namespace Steering
	{
		void Test::tick(Scene &scene)
		{
			scene.eachEntity<Components::Steering::Test, Components::Locomotion::Heading, Components::Transform>(
						[this](auto &entity, const auto &test, auto &heading, const auto &transform) {
						//Vector2 in = AI::Steering::Behavior::seek(transform.pos, raylib::Vector2(200, 400), 250);
						Vector2 in = AI::Steering::Behavior::arrive(transform.pos, raylib::Vector2(200, 400));

						entity.template update<Components::Locomotion::Heading>([&in](auto &heading) {
							heading.dir = in;
						});
			});
		}

	}

}
