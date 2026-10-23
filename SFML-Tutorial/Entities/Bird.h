#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Bird {
private:
    RectangleShape shape;
    float speed;
    float leftBound;
    float rightBound;

    int direction = 1;
    bool isKilled = false;
    int currentFrame = 0;

    Clock animationClock;
    static const int FRAME_WIDTH = 48;
    static const int FRAME_HEIGHT = 48;
    static const int FRAME_COUNT = 9;

public:
    Bird(Vector2f position, Vector2f size, float moveSpeed, float range, const Texture& texture);

    void update(float deltaTime);

    void draw(RenderWindow& window);

    void kill();

    bool isDead() const;

    Vector2f getPosition() const;

    Vector2f getSize() const;

    FloatRect getBounds() const;
};
