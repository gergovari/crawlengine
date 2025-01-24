#pragma once

#include "components/components.hpp"

class Entity;
class Camera2D;
class Rectangle;
class Scene;
class Renderables;

bool IsRectValid(Rectangle rect);
Rectangle GetCameraView(Camera2D &camera);
void DrawRenderables(Camera2D &cam, Renderables &renderables);
std::array<Rectangle, 8> NeighbouringColliders(Scene &scene, Rectangle collider);
bool IsColliding(Scene &scene, Rectangle collider, Rectangle &result);
Rectangle TcToRect(const Components::Transform &transform, const Components::Collider &collider);
Rectangle TaToRect(const Components::Transform &transform, const Components::Area &area);
