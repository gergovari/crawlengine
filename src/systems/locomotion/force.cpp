#include "systems/locomotion/force.hpp"

#include "entity/entity.hpp"
#include "misc/raylib_extended.hpp"
#include "scene/scene.hpp"

#include "components/transform.hpp"
#include "components/collider.hpp"

#include "components/locomotion/force.hpp"
#include "components/locomotion/heading.hpp"

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
					auto gravityAccel = 1000;
					auto frictionCoeff = .9;

					auto inForce = heading.dir.Scale(locomotion.maxForce);
					auto friction = locomotion.vel.Normalize().Scale(frictionCoeff * locomotion.mass * gravityAccel);
					auto force = inForce - friction;

					auto accel = force.Scale(1/locomotion.mass);
					entity.template update<Components::Locomotion::Force>([&delta, &heading, &accel](auto &locomotion) {
						locomotion.vel += accel.Scale(delta);
						locomotion.vel = locomotion.vel.Clamp(
								-locomotion.maxVelocity * heading.maxVelocityScale, 
								locomotion.maxVelocity * heading.maxVelocityScale);
					});
					auto vel = locomotion.vel;
					
					if (vel.Length() > 15) {
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
					}
			});
		}
	}
}
