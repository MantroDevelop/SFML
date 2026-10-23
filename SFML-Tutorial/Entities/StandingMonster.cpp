#include "StandingMonster.h"

using namespace sf;

StandingMonster::StandingMonster(Vector2f position, Vector2f size, const Texture& texture, Platform* platform)
    : attachedPlatform(platform)
{
    shape.setSize(size);
    shape.setTexture(&texture);
    shape.setOrigin(shape.getGeometricCenter());
    shape.setPosition(position);
}

void StandingMonster::update()
{
    if (attachedPlatform != nullptr)
    {
        Vector2f platformPosition = attachedPlatform->getPosition();
        shape.setPosition({ platformPosition.x, platformPosition.y - 36.f });
    }
}

void StandingMonster::draw(RenderWindow& window) const
{
    if (!isKilled)
    {
        window.draw(shape);
    }
}

void StandingMonster::kill()
{
    isKilled = true;
}

bool StandingMonster::isDead() const
{
    return isKilled;
}

Vector2f StandingMonster::getPosition() const
{
    return shape.getPosition();
}

Vector2f StandingMonster::getSize() const
{
    return shape.getSize();
}

FloatRect StandingMonster::getBounds() const
{
    FloatRect bounds = shape.getGlobalBounds();
    float insetX = 10.f;
    float insetY = 8.f;

    return FloatRect(
        { bounds.position.x + insetX, bounds.position.y + insetY },
        { bounds.size.x - insetX * 2.f, bounds.size.y - insetY * 2.f }
    );
}
