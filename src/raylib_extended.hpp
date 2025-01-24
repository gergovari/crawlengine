#pragma once

#include "entity.hpp"
#include "raylib.h"
#include "renderables.hpp"
#include "scene.hpp"
#include <array>

bool IsRectValid(Rectangle rect);
Rectangle GetCameraView(Camera2D &camera);
void DrawRenderables(Camera2D &cam, Renderables &renderables);
std::array<Rectangle, 8> NeighbouringColliders(Scene &scene, Rectangle collider);
bool IsColliding(Scene &scene, Rectangle collider, Rectangle &result);
Rectangle TcToRect(const Component::Transform &transform, const Component::Collider &collider);
Rectangle TaToRect(const Component::Transform &transform, const Component::Area &area);
