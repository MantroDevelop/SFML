#include "BreakablePlatform.h"

using namespace sf;

BreakablePlatform::BreakablePlatform(Vector2f position, Vector2f size, const Texture& texture, int skinIndex)
    : Platform(position, size, texture, skinIndex)
{
}

void BreakablePlatform::breakPlatform()
{
    isBroken = true;
}

bool BreakablePlatform::isGetBroken() const
{
    return isBroken;
}

void BreakablePlatform::draw(RenderWindow& window) const
{
    if (!isBroken)
    {
        window.draw(shape);
    }
}
