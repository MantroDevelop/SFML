#include "PowerUp.h"
#include "BreakablePlatform.h"

using namespace sf;

PowerUp::PowerUp(Vector2f position, Vector2f size, Platform* attachedPlatformptr, const Texture& texture)
    : attachedPlatform(attachedPlatformptr)
{
    shape.setSize(size);
    shape.setTexture(&texture);
    shape.setOrigin(shape.getGeometricCenter());
    shape.setPosition(position);
}

void PowerUp::draw(RenderWindow& window)
{
    if (!isCollected)
    {
        window.draw(shape);
    }
}

void PowerUp::collectPower()
{
    isCollected = true;
}

void PowerUp::update()
{
    shape.setPosition({ attachedPlatform->getPosition().x, attachedPlatform->getPosition().y - 34.f });
}

bool PowerUp::isPlatformBroken() const
{
    BreakablePlatform* breakable = dynamic_cast<BreakablePlatform*>(attachedPlatform);
    return breakable != nullptr && breakable->isGetBroken();
}

Vector2f PowerUp::getAttachedPlatformPostion()
{
    return attachedPlatform->getPosition();
}

FloatRect PowerUp::getBounds() const
{
    return shape.getGlobalBounds();
}

bool PowerUp::isGetCollected() const
{
    return isCollected;
}

Vector2f PowerUp::getPosition() const
{
    return shape.getPosition();
}

Vector2f PowerUp::getSize() const
{
    return shape.getSize();
}
