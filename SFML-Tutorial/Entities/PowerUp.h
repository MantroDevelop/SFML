#pragma once
#include "Platform.h"

using namespace sf;

class PowerUp {
private:
    Platform* attachedPlatform;
    RectangleShape shape;
    bool isCollected = false;

public:
    PowerUp(Vector2f position, Vector2f size, Platform* attachedPlatformptr, const Texture& texture);

    void draw(RenderWindow& window);

    void collectPower();

    void update();

    bool isPlatformBroken() const;

    Vector2f getAttachedPlatformPostion();

    FloatRect getBounds() const;

    bool isGetCollected() const;

    Vector2f getPosition() const;

    Vector2f getSize() const;
};
