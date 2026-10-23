#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Player {
private:
    Vector2f position;
    Vector2f velocity;
    Vector2f size;
    float speed = 250.f;

public:
    Player(Vector2f startPosition, Vector2f playerSize, float moveSpeed);

    void jump();

    void superJump();

    void update(float deltaTime, float gravity);

    void handleInput(float deltaTime, Sprite& player);

    FloatRect getBounds() const;

    void wrapX(float windowWidth);

    void landOn(float platformTopY);

    void reset(Vector2f startPosition);

    Vector2f getPosition() const;

    Vector2f getVelocity() const;

    Vector2f getSize() const;
};
