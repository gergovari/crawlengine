#pragma once

#include "components/transform.hpp"
#include "components/collider.hpp"
#include "components/area.hpp"

class Entity;
class Camera2D;
class Rectangle;
class Scene;

bool IsRectValid(Rectangle rect);
Rectangle GetCameraView(Camera2D &camera);
std::array<Rectangle, 8> NeighbouringColliders(Scene &scene, Rectangle collider);
bool IsColliding(Scene &scene, Rectangle collider, Rectangle &result);
Rectangle TcToRect(const Components::Transform &transform, const Components::Collider &collider);
Rectangle TaToRect(const Components::Transform &transform, const Components::Area &area);
