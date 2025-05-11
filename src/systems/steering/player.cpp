#include "systems/steering/player.hpp"

#include "components/components.hpp"
#include "scene/scene.hpp"

#include "services/services.hpp"

namespace Systems
{
	namespace Steering
	{
		void Player::tick(Scene &scene)
		{
			auto &input = entt::locator<Services::Inputs::Input>::value();

			scene.eachEntity<Components::Steering::Player, Components::Locomotion::Velocity, Components::Locomotion::Heading>(
					[&input](auto &entity, const auto &steering, auto &locomotion, auto &heading) {
					using namespace Services::Inputs;

					entity.template update<Components::Locomotion::Velocity>([&input](auto &locomotion) {
						if (input.isDown(MOVE_SPRINT))
						{
							locomotion.speed = SPRINT_SPEED;
						}
						else
						{
							locomotion.speed = WALK_SPEED;
						}
					});

					entity.template update<Components::Locomotion::Heading>([&input](auto &heading) {
						heading.dir = raylib::Vector2((float)(input.isDown(MOVE_RIGHT) - input.isDown(MOVE_LEFT)),
								(float)(input.isDown(MOVE_DOWN) - input.isDown(MOVE_UP)));
						});
					});

		}
	}
}
