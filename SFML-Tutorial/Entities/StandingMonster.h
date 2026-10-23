#pragma once
#include "Platform.h"

using namespace sf;

class StandingMonster {
private:
    RectangleShape shape;
    bool isKilled = false;
    Platform* attachedPlatform;

public:
    StandingMonster(Vector2f position, Vector2f size, const Texture& texture, Platform* platform);

    void update();

    void draw(RenderWindow& window) const;

    void kill();

    bool isDead() const;

    Vector2f getPosition() const;

    Vector2f getSize() const;

    FloatRect getBounds() const;
};

