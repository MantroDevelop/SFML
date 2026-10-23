#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Platform {
protected:
    RectangleShape shape;

public:
    Platform(Vector2f position, Vector2f size, const Texture& texture, int skinIndex);

    virtual void draw(RenderWindow& window) const;

    virtual ~Platform();

    virtual void update(float deltaTime);

    Vector2f getPosition() const;

    Vector2f getSize() const;

    FloatRect getBounds() const;
};
