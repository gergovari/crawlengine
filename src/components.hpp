#pragma once

#include <entt/entt.hpp>

#include "raylib.h"
#include "raymath.h"

struct Entity;

namespace Component
{

struct Camera
{
    Camera2D cam = {0};
    Entity *target = nullptr;
    Vector2 offset = {0};
};

struct Transform
{
    Vector2 pos = {0};
    operator Vector2() const
    {
        return pos;
    }
};

struct ColoredRect
{
    Color color = DARKPURPLE;
    Vector2 size = {0};
};

struct Locomotion
{
    Vector2 vel = {0};
    float targetSpeed = 0;
    std::vector<std::function<Vector2(Vector2)>> modifiers;

    void setVelocity(Vector2 v);
};

struct Collider
{
    Vector2 size = {0};
};

struct Area
{
    Vector2 size = {0};
    entt::dispatcher dispatcher{};
};

struct MovementSlowDown
{
    float multiplier = 0.5f;
};

namespace Steering
{
struct Player
{
    int dummy = 0;
};

struct Test
{
    int speed = 0;
};
} // namespace Steering

} // namespace Component

namespace Tag
{
struct Renderable
{
    int z = 0;

    /* TODO: deprecate as soon as EnTT
     * implements enabled/disabled components */
    bool render = true;
};
} // namespace Tag
