#include "systems/steering/player.hpp"

#include "scene/scene.hpp"

#include "services/inputs/input.hpp"

#include "components/locomotion/heading.hpp"
#include "components/steering/player.hpp"

namespace Systems
{
	namespace Steering
	{
		void Player::tick(Scene &scene)
		{
			auto &input = entt::locator<Services::Inputs::Input>::value();

			scene.eachEntity<Components::Steering::Player, Components::Locomotion::Heading>(
					[&input](auto &entity, const auto &steering, auto &heading) {
					using namespace Services::Inputs;

					const auto baseVelocity = 100;
					auto dirScale = input.isDown(MOVE_SPRINT) ? 1 : 0.5;
					auto maxVelocityScale = input.isDown(MOVE_SPRINT) ? 1 : .5;

					entity.template update<Components::Locomotion::Heading>([&dirScale, &maxVelocityScale, &input](auto &heading) {
						heading.dir = raylib::Vector2((float)(input.isDown(MOVE_RIGHT) - input.isDown(MOVE_LEFT)),
								(float)(input.isDown(MOVE_DOWN) - input.isDown(MOVE_UP))).Scale(dirScale);
						});
						heading.maxVelocityScale = maxVelocityScale;
					});

		}
	}
}
