#include "Bird.h"
#include <algorithm>

using namespace sf;
using namespace std;

Bird::Bird(Vector2f position, Vector2f size, float moveSpeed, float range, const Texture& texture)
{
    shape.setSize(size);
    shape.setTexture(&texture);
    shape.setTextureRect(IntRect({ 0, 0 }, { FRAME_WIDTH, FRAME_HEIGHT }));
    shape.setOrigin(shape.getGeometricCenter());
    shape.setPosition(position);
    speed = moveSpeed;
    leftBound = max(position.x - range, 60.f);
    rightBound = min(position.x + range, 540.f);
}

void Bird::update(float deltaTime)
{
    if (isKilled)
    {
        return;
    }

    float dx = speed * direction * deltaTime;
    shape.move({ dx, 0.f });

    if (shape.getPosition().x <= leftBound || shape.getPosition().x >= rightBound)
    {
        direction *= -1;
    }

    if (direction == 1)
    {
        shape.setScale({ -1.75f, 1.75f });
    }

    if (direction == -1)
    {
        shape.setScale({ 1.75f, 1.75f });
    }

    if (animationClock.getElapsedTime().asSeconds() > 0.1f)
    {
        currentFrame++;

        if (currentFrame >= FRAME_COUNT)
        {
            currentFrame = 0;
        }

        shape.setTextureRect(IntRect({ currentFrame * 64, 0 }, { FRAME_WIDTH, FRAME_HEIGHT }));
        animationClock.restart();
    }
}

void Bird::draw(RenderWindow& window)
{
    if (!isKilled)
    {
        window.draw(shape);
    }
}

void Bird::kill()
{
    isKilled = true;
}

bool Bird::isDead() const
{
    return isKilled;
}

Vector2f Bird::getPosition() const
{
    return shape.getPosition();
}

Vector2f Bird::getSize() const
{
    return shape.getSize();
}

FloatRect Bird::getBounds() const
{
    float width = 38.f;
    float height = 30.f;
    float offsetX = 8.f;
    float offsetY = 10.f;

    if (direction == 1)
    {
        offsetX = -8.f;
    }

    return FloatRect(
        { shape.getPosition().x - width / 2.f + offsetX,
          shape.getPosition().y - height / 2.f + offsetY },
        { width, height }
    );
}
