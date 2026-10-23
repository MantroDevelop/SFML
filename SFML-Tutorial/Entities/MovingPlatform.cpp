#include "MovingPlatform.h"

using namespace sf;

MovingPlatform::MovingPlatform(Vector2f position, Vector2f size, float moveSpeed, float range, const Texture& texture, int skinIndex)
    : Platform(position, size, texture, skinIndex), speed(moveSpeed),
      leftBound(position.x - range), rightBound(position.x + range)
{
}

void MovingPlatform::update(float deltaTime)
{
    float dx = speed * direction * deltaTime;
    shape.move({ dx, 0.f });

    if (shape.getPosition().x <= leftBound || shape.getPosition().x >= rightBound ||
        shape.getPosition().x - 50.f <= 0.f || shape.getPosition().x + 50.f >= 600.f)
    {
        direction *= -1;
    }
}
