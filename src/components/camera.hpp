
class Camera2D;
class Entity;
class Vector2;

namespace Component
{
    struct Camera
    {
        Camera2D cam = {0};
        Entity *target = nullptr;
        Vector2 offset = {0};
    };
}
