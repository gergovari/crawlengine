#include "systems/locomotion/velocity.hpp"

#include "entity/entity.hpp"
#include "misc/raylib_extended.hpp"
#include "scene/scene.hpp"

#include "components/transform.hpp"
#include "components/collider.hpp"
#include "components/locomotion/velocity.hpp"
#include "components/locomotion/heading.hpp"

namespace Systems
{
	namespace Locomotion
	{
		void Velocity::tick(Scene &scene)
		{
			scene.eachEntity<Components::Transform, 
				Components::Locomotion::Velocity, Components::Locomotion::Heading, Components::Collider>(
					[&scene](auto &entity, const auto &transform, const auto &locomotion, const auto &heading, const auto &collider) {
					Rectangle collision;

					if (heading.dir.LengthSqr() != 0)
					{
						raylib::Vector2 vel = heading.dir.Scale(locomotion.speed).Scale(GetFrameTime());

						entity.template update<Components::Transform>([&vel, &locomotion](auto &transform) {
								transform.pos.x += vel.x * locomotion.multiplier;
								});
						if (IsColliding(scene, TcToRect(transform, collider), collision))
						{
							entity.template update<Components::Transform>([&vel, &collision, &locomotion](auto &transform) {
									transform.pos.x -= collision.width * ((vel.x > 0) - (vel.x < 0));
									});
						}
						entity.template update<Components::Transform>([&vel, &locomotion](auto &transform) {
								transform.pos.y += vel.y * locomotion.multiplier;
								});

						if (IsColliding(scene, TcToRect(transform, collider), collision))
						{
							entity.template update<Components::Transform>([&vel, &collision, &locomotion](auto &transform) {
									transform.pos.y -= collision.height * ((vel.y > 0) - (vel.y < 0));
									});
						}
					}
					});
		}
	}
}
