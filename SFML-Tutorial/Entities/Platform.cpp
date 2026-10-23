#include "Platform.h"

using namespace sf;

Platform::Platform(Vector2f position, Vector2f size, const Texture& texture, int skinIndex)
{
    shape.setSize(size);
    shape.setTexture(&texture);
    shape.setTextureRect(IntRect({ 0, skinIndex * 16 }, { 33, 16 }));
    shape.setOrigin(shape.getGeometricCenter());
    shape.setPosition(position);
}

void Platform::draw(RenderWindow& window) const
{
    window.draw(shape);
}

Platform::~Platform(){}

void Platform::update(float deltaTime)
{
}

Vector2f Platform::getPosition() const
{
    return shape.getPosition();
}

Vector2f Platform::getSize() const
{
    return shape.getSize();
}

FloatRect Platform::getBounds() const
{
    return shape.getGlobalBounds();
}
