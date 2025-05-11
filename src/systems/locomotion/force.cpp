#include "systems/locomotion/force.hpp"

#include "entity/entity.hpp"
#include "misc/raylib_extended.hpp"
#include "scene/scene.hpp"

namespace Systems
{
	namespace Locomotion
	{
		void Force::tick(Scene &scene)
		{
			scene.eachEntity<Components::Transform, Components::Locomotion::Force, Components::Locomotion::Heading, Components::Collider>(
				[&scene](auto &entity, const auto &transform, const auto &locomotion, const auto &heading, const auto &collider) {
					Rectangle collision;
					auto delta = GetFrameTime();

					if (heading.dir.LengthSqr() != 0)
					{	
						auto accel = heading.dir.Scale(locomotion.max).Scale(1/locomotion.mass);
						entity.template update<Components::Locomotion::Force>([&delta, &accel](auto &locomotion) {
							locomotion.vel += accel.Scale(delta);
						});
					}
					auto vel = locomotion.vel;

					entity.template update<Components::Transform>([&delta, &vel, &locomotion](auto &transform) {
							transform.pos.x += vel.x * delta;
					});

					if (IsColliding(scene, TcToRect(transform, collider), collision))
					{
						entity.template update<Components::Transform>([&vel, &collision, &locomotion](auto &transform) {
								transform.pos.x -= collision.width * ((vel.x > 0) - (vel.x < 0));
						});
						entity.template update<Components::Locomotion::Force>([](auto &locomotion) {
							locomotion.vel.x = 0;
						});
					}

					entity.template update<Components::Transform>([&delta, &vel, &locomotion](auto &transform) {
							transform.pos.y += vel.y * delta;
					});

					if (IsColliding(scene, TcToRect(transform, collider), collision))
					{
						entity.template update<Components::Transform>([&vel, &collision, &locomotion](auto &transform) {
								transform.pos.y -= collision.height * ((vel.y > 0) - (vel.y < 0));
						});
						entity.template update<Components::Locomotion::Force>([](auto &locomotion) {
							locomotion.vel.y = 0;
						});
					}
			});
		}
	}
}
