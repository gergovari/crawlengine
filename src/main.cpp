#include <cstdio>

#include <entt/entity/registry.hpp>

#include "raylib.h"
#include "raymath.h"

#include <forward_list>
#include <unordered_set>

#define WINDOW_TITLE "CrawlSTART"

#define SCREEN_WIDTH (1280)
#define SCREEN_HEIGHT (720)

#define WORLD_WIDTH 100
#define WORLD_HEIGHT 100
#define TILE_SIZE 60.0f

#define PLAYER_SIZE 50.0f
#define PLAYER_SPAWN_X 2
#define PLAYER_SPAWN_Y 1
#define WALK_SPEED 120.0f
#define SPRINT_SPEED WALK_SPEED * 2

typedef std::pair<int, int> SpatialPair;

template <typename T>
using SpatialCell = typename std::forward_list<T>;

struct SpatialPairHash
{
	std::size_t operator() (const SpatialPair &pair) const {
		size_t lhs = pair.first;
		const size_t rhs = pair.second;

		lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
		return lhs;
	}
};

template <typename T>
class SpatialHashMap {
	private:
		/* TODO: think about load factor and resizing */
		std::unordered_map<SpatialPair, SpatialCell<T>, SpatialPairHash> cells;
		const SpatialPair unit;
		
		void forEach(std::function<bool(SpatialCell<T>&)> func, Rectangle rect)
		{
			auto steps = std::make_pair((int)std::floor(rect.width / unit.first), (int)std::floor(rect.height / unit.second));
			auto base = std::make_pair((int)std::floor(rect.x / unit.first), (int)std::floor(rect.y / unit.second));

			for (int i = 0; i < steps.first; i++) {
				for (int j = 0; j < steps.second; j++) {
					const auto pos = std::make_pair(base.first + i, base.second + j);

					if (func(cells[pos])) {
						return;
					}
				}
			}

		}

		void forEach(std::function<bool(SpatialCell<T>&)> func)
		{
			for (auto &[pos, cell] : cells) {
				if (func(cell)) {
					return;
				}
			}
		}
	public:
		SpatialHashMap(SpatialPair b, SpatialPair u) : unit(u) {}

		void insert(T item, Rectangle rect)
		{
			forEach([&item, &rect](auto &cell) {
				cell.emplace_front(item);
				return false;
			}, rect);
		}
		
		void update(T item, Rectangle rect) 
		{
			remove(item);
			insert(item, rect);
		}

		void replace(T item, T t) 
		{
			forEach([&item, &t](auto &cell) {
				for (auto &current : cell) {
					if (current == item) {
						current = t;
						return true;
					}
				}
				return false;
			});
		}

		void remove(T item)
		{
			forEach([&item](auto &cell) {
				auto prev = cell.before_begin();

				for (auto it = cell.begin(); it != cell.end(); ++it) {
					if (*it == item) {
						it = cell.erase_after(prev);
						return true;
					}

					prev = it;
				}
				return false;
			});
		}

		SpatialCell<T> nearby(Rectangle rect)
		{
			SpatialCell<T> result;

			forEach([&result](auto &cell) {
				for (auto &item : cell) {
					result.push_front(item);
				}
				return false;
			}, rect);

			return result;
		}
};

static const inline Rectangle GetCameraView(Camera2D &camera)
{
	float scale = 1.0f / camera.zoom;

	Rectangle view;
	view.x = camera.target.x - (camera.offset.x * scale);
	view.y = camera.target.y - (camera.offset.y * scale);
	view.width = SCREEN_WIDTH * scale;
	view.height = SCREEN_HEIGHT * scale;

	return view;
}

class Entity {
	private:
		entt::registry *registry;
		entt::entity entity;

	public:

		Entity(entt::registry *r) : registry(r) {
			entity = registry->create();
		}

		~Entity() {
			registry->destroy(entity);
		}
	
		bool operator==(const Entity &other) const
		{
			return (registry == other.registry) && (entity == other.entity);
		}

		template<typename Type, typename... Args>
			decltype(auto) add(Args &&...args)
			{
				return registry->emplace<Type>(entity, std::forward<Args>(args)...);
			}

		template<typename Type, typename... Args>
			decltype(auto) update(Args &&...args)
			{
				return registry->patch<Type>(entity, std::forward<Args>(args)...);
			}

		template<typename Type>
			decltype(auto) get() const
			{
				return registry->get<Type>(entity);
			}

		template<typename... Args>
			decltype(auto) has() const
			{
				return registry->all_of<Args...>(entity);
			}

		template<typename... Args>
			decltype(auto) remove()
			{
				return registry->remove<Args...>(entity);
			}
};

template <typename T>
struct RenderableItem {
	T item;
	int z;

	bool operator==(const RenderableItem<T> &other) const
	{
		return item == other.item;
	}
};

#define SPATIAL_UNIT 50
class Scene {
	private:
		entt::registry registry;

		static const RenderableItem<Entity> inline eToRI(Entity entity)
		{
			return RenderableItem<Entity>{ entity, entity.get<RenderableTag>().z };
		}

		static const RenderableItem<Entity> inline eToRI(Entity entity, int z)
		{
			return RenderableItem<Entity>{ entity, z };
		}

		void onDestroyRenderable(SpatialHashMap<RenderableItem<Entity>> &renderables, entt::registry& registry, entt::entity e)
		{
			Entity entity(&registry, e);

			renderables.remove(eToRI(entity));
		}

		void onConstructRenderable(SpatialHashMap<RenderableItem<Entity>> &renderables, entt::registry& registry, entt::entity e)
		{
			Entity entity(&registry, e);

			if (entity.has<TransformComponent>()) {
				auto &transform = entity.get<TransformComponent>();

				if (entity.has<ColoredRectComponent>()) {
					auto &coloredRect = entity.get<ColoredRectComponent>();

					renderables.insert(eToRI(entity), { transform.pos.x, transform.pos.y, coloredRect.size.x, coloredRect.size.y });
				}
			}
		}

		void onUpdateTransform(SpatialHashMap<RenderableItem<Entity>> &renderables, entt::registry& registry, entt::entity e)
		{
			Entity entity(&registry, e);
			
			if (entity.has<RenderableTag>()) {
				auto &transform = entity.get<TransformComponent>();

				if (entity.has<ColoredRectComponent>()) {
					auto &coloredRect = entity.get<ColoredRectComponent>();

					renderables.update(eToRI(entity), { transform.pos.x, transform.pos.y, coloredRect.size.x, coloredRect.size.y });
				}
			}
		}

		void onUpdateRenderable(SpatialHashMap<RenderableItem<Entity>> &renderables, entt::registry& registry, entt::entity e)
		{
			Entity entity(&registry, e);

			if (entity.has<TransformComponent>()) {
				auto &transform = entity.get<TransformComponent>();

				if (entity.has<ColoredRectComponent>()) {
					auto &coloredRect = entity.get<ColoredRectComponent>();
					const auto renderable = eToRI(entity);
					
					/* If you observe RenderableItem, 
					 * we don't need the Z values to equal for a match. 
					 * Thus we don't need the old one. */
					renderables.replace(renderable, renderable);
				}
			}
		}

	public:
		std::vector<Entity> entities;
		SpatialHashMap<RenderableItem<Entity>> renderables(std::make_pair(SPATIAL_UNIT, SPATIAL_UNIT));

		Scene() {
			/* TODO: reactive storage https://github.com/skypjack/entt/wiki/Entity-Component-System#storage */
			registry.on_construct<RenderableTag>().connect<&onConstructRenderable>(renderables);
			registry.on_destroy<RenderableTag>().connect<&onDestroyRenderable>(renderables);
			registry.on_update<RenderableTag>().connect<&onUpdateRenderable>(renderables);
			registry.on_update<TransformComponent>().connect<&onUpdateTransform>(renderables);
		}
		
		Entity& add()
		{
			return entities.emplace_back(&registry);
		}

		void remove(const Entity &entity)
		{
			if (std::find(entities.begin(), entities.end(), entity) != entities.end()) {
				entities.erase(it);
			}
		}
		
		template <typename... ComponentTypes>
		void each(std::function<void(Entity&, ComponentTypes&...)> lambda)
		{
			auto view = registry->view<ComponentTypes...>();

			view.each([&](entt::entity e, ComponentTypes&... components) {
					Entity entity(registry, e);

					lambda(entity, components...);
			});
		}

		template <typename... ComponentTypes>
		void each(std::function<void(const ComponentTypes&...)> lambda)
		{
			auto view = registry->view<ComponentTypes...>();

			view.each([&](ComponentTypes&... components) {
					lambda(components...);
			});
		}
};

struct CameraComponent {
	Camera2D cam = { 0 };

	Entity* target = nullptr;
	Vector2 offset = { 0 };
};

struct TransformComponent {
	Vector2 pos = { 0 };

	operator Vector2() const { return pos; }
};

struct ColoredRectComponent {
	Color color = DARKPURPLE;
	Vector2 size = { 0 };
};

struct LocomotionComponent {
	Vector2 vel = { 0 };
	float targetSpeed = 0;
	float multiplier = 1;

	void setVelocity(Vector2 v)
	{
		vel = Vector2Clamp(v, { -targetSpeed, -targetSpeed }, { targetSpeed, targetSpeed });
		vel = Vector2Scale(vel, GetFrameTime());
	}
};

struct PlayerSteeringComponent {
	int dummy = 0;
};

struct TestSteeringComponent {
	int speed = WALK_SPEED;
};

struct ColliderComponent {
	Vector2 size = { 0 };
};

struct RenderableTag {
	int z = 0;

	/* TODO: deprecate as soon as EnTT 
	 * implements enabled/disabled components */
	bool render = true;
};

void drawRenderables(Camera2D &cam, SpatialHashMap<RenderableItem<Entity>> &renderables)
{
	auto view = GetCameraView(cam);
	
	view.x -= TILE_SIZE;
	view.y -= TILE_SIZE;
	view.width += 3 * TILE_SIZE;
	view.height += 3 * TILE_SIZE;
	
	auto items = renderables.nearby(view);

	items.sort([](auto const &a, auto const &b) {
		return a.z < b.z;
	});

	for (auto &renderable : items) {
		Entity& entity = renderable.item;
		auto &transform = entity.get<TransformComponent>();

		if (entity.has<ColoredRectComponent>()) {
			auto &rect = entity.get<ColoredRectComponent>();

			DrawRectangleV(transform.pos, rect.size, rect.color);
		}
	}
}

static inline void renderToCameras(Scene &scene)
{
	auto &renderables = scene.renderables;

	scene.each<CameraComponent>([&renderables](auto &comp) {
		auto &cam = comp.cam;

		BeginMode2D(cam);
		drawRenderables(cam, renderables);
		EndMode2D();
	});
}

static inline void followCameraTargets(Scene &scene)
{
	scene.each<CameraComponent>([](auto &comp) {
		auto &cam = comp.cam;
		auto &entity = *comp.target;
		
		if (entity != nullptr) {
			if (entity.has<TransformComponent>()) {
				cam.target = Vector2Add(entity.get<TransformComponent>(), comp.offset);
			}
		}
	});
}

static const inline std::array<Rectangle, 8> neighbouringColliders(Scene &scene, Rectangle collider)
{
	const Vector2 transform = { collider.x + collider.width / 2, collider.y + collider.height / 2 };
	const std::array<Vector2, 8> offsets = {
		{ -collider.width, -collider.height },
		{ -collider.width, 0 },
		{ -collider.width, collider.height },
		{ 0, -collider.height },
		{ 0, collider.height },
		{ collider.width, -collider.height },
		{ collider.width, 0 },
		{ collider.width, collider.height }
	};
	std::array<Rectangle, 8> found;
	size_t count = 0;

	scene.each<CameraComponent>([&offsets](auto &tilePos, auto &collider) {
		for (const auto &offset : offsets) {
			const Rectangle collision = { tilePos.pos.x, tilePos.pos.y, collider.size.x, collider.size.y };

			if (CheckCollisionPointRec(Vector2Add(transform, offset), collision)) {
				if (found.size() <= count) {
					return;
				}
				found[count++] = collision;
			}
		}
	});
}

bool isColliding(Scene &scene, Rectangle collider, Rectangle &result)
{
	const auto found = neighbouringColliders(scene);

	for (const auto &collision : found) {
		result = GetCollisionRec(collider, collision);
		
		return result.width > 0 && result.height > 0;
	}
	return false;
}

static inline Rectangle tcToRect(TransformComponent transform, ColliderComponent collider)
{
	return { transform.pos.x, transform.pos.y, collider.size.x, collider.size.y };
}

void handleLocomotion(Scene &scene)
{
	scene.each<TransformComponent, LocomotionComponent, ColliderComponent>([](auto &entity, const auto &transform, const auto &locomotion, const auto &collider) {
		Rectangle collision;

		/* We apply velocity to coordinates separately 
		 * as otherwise we would encounter strange 
		 * behaviour with collisions. */
		if (Vector2LengthSqr(locomotion.vel) != 0) {
			entity.update<TransformComponent>([&locomotion](auto &transform) {
				transform.pos.x += locomotion.vel.x * locomotion.multiplier;
			});

			if (isColliding(scene, tcToRect(transform, collider), &collision)) {
				entity.update<TransformComponent>([&collision, &locomotion](auto &transform) {
					transform.pos.x -= collision.width * ((locomotion.vel.x > 0) - (locomotion.vel.x < 0));
				});
			}

			entity.update<TransformComponent>([&locomotion](auto &transform) {
				transform.pos.y += locomotion.vel.y * locomotion.multiplier;
			});

			if (isColliding(scene, tcToRect(transform, collider), &collision)) {
				entity.update<TransformComponent>([&collision, &locomotion](auto &transform) {
					transform.pos.y -= collision.height * ((locomotion.vel.y > 0) - (locomotion.vel.y < 0));
				});
			}
		}
	});
}

void handlePlayerSteering(Scene &scene)
{
	scene.each<PlayerSteeringComponent, LocomotionComponent>([](const auto &input, auto &locomotion) {
		/* input will be used when`
		 * I get into multiple input devices, 
		 * custom keybinds and many more advanced ideas... */

		if (IsKeyDown(KEY_LEFT_SHIFT)) {
			locomotion.targetSpeed = SPRINT_SPEED;
		} else {
			locomotion.targetSpeed = WALK_SPEED;
		}
		Vector2 in = { (float)(IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT)), (float)(IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP)) };
		
		locomotion.setVelocity(Vector2Scale(in, locomotion.targetSpeed));
	});
}

void handleTestSteering(Scene &scene)
{
	scene.each<TestSteeringComponent, LocomotionComponent>([](const auto &test, auto &locomotion) {
		locomotion.targetSpeed = test.speed;
		Vector2 in;

		if ((int)GetTime() % 2 == 0) {
			in = { 1, 0 };
		} else if ((int)GetTime() % 3 == 0) {
			in = { 0, 1 };
		} else if ((int)GetTime() % 8 == 0) {
			in = { -1, 0 };
		} else {
			in = { 0, -1 };
		}
		
		locomotion.setVelocity(Vector2Scale(in, locomotion.targetSpeed));
	});
}

void addWorld(Scene &scene) {
	for (int x = 0; x < WORLD_WIDTH; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			int i = y + x * WORLD_HEIGHT;
			auto &entity = scene.add();

			if (x == 0 || x == WORLD_WIDTH - 1 || y == 0 || y == WORLD_HEIGHT - 1 || (x == 5 && (y > 3 && y < 9)) || (x == 8 && (y > 3 && y < 9))) {
				entity.add<TransformComponent>(Vector2{ x * TILE_SIZE, y * TILE_SIZE });
				entity.add<ColliderComponent>(Vector2{ TILE_SIZE, TILE_SIZE });
				entity.add<ColoredRectComponent>(GRAY, Vector2{ TILE_SIZE, TILE_SIZE });
				entity.add<RenderableTag>();
			} else if (x == 10) {
				entity.add<TransformComponent>(Vector2{ x * TILE_SIZE, y * TILE_SIZE });
				entity.add<ColoredRectComponent>(DARKBLUE, Vector2{ TILE_SIZE, TILE_SIZE });
				entity.add<RenderableTag>(-10);
			} else {
				entity.add<TransformComponent>(Vector2{ x * TILE_SIZE, y * TILE_SIZE });
				entity.add<ColoredRectComponent>(DARKGRAY, Vector2{ TILE_SIZE, TILE_SIZE });
				entity.add<RenderableTag>(-10);
			}
		}
	}
}

void addPlayer(Scene &scene) {
	auto &entity = scene.add();

	entity.add<TransformComponent>(Vector2{ PLAYER_SPAWN_X * TILE_SIZE, PLAYER_SPAWN_Y * TILE_SIZE });
	entity.add<LocomotionComponent>();
	entity.add<ColliderComponent>(Vector2{ PLAYER_SIZE, PLAYER_SIZE });
	entity.add<PlayerSteeringComponent>();

	entity.add<ColoredRectComponent>(GREEN, Vector2{ PLAYER_SIZE, PLAYER_SIZE });
	entity.add<RenderableTag>();

	entity.add<CameraComponent>();
	CameraComponent &comp = entity.get<CameraComponent>();
	Camera2D *cam = &comp.cam;
	
	comp.target = &entity;
	comp.offset = { PLAYER_SIZE / 2, PLAYER_SIZE / 2 };

	cam->target = { 0, 0 };
	cam->offset = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
	cam->rotation = 0.0f;
	cam->zoom = 1.0f;
}

void addEnemy(Scene &scene) {
	auto &entity = scene.add();

	entity.add<TransformComponent>(Vector2{ 3 * TILE_SIZE, 4 * TILE_SIZE });
	entity.add<LocomotionComponent>();
	entity.add<TestSteeringComponent>();
	entity.add<ColliderComponent>(Vector2{ PLAYER_SIZE, PLAYER_SIZE });

	entity.add<ColoredRectComponent>(RED, Vector2{ PLAYER_SIZE, PLAYER_SIZE });
	entity.add<RenderableTag>();
}

int main()
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
	SetTargetFPS(60);
	
	Scene scene;
	
	addWorld(scene);
	addPlayer(scene);
	addEnemy(scene);

	while (!WindowShouldClose())
	{
		handlePlayerSteering(scene);
		handleTestSteering(scene);
		handleLocomotion(scene);

		followCameraTargets(scene);

		BeginDrawing();
		ClearBackground(RAYWHITE);

		renderToCameras(scene);
		DrawFPS(0, 0);
		
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
