#pragma once
#include "Platform.h"

using namespace sf;

class MovingPlatform : public Platform {
private:
    float speed;
    float leftBound;
    float rightBound;
    int direction = 1;

public:
    MovingPlatform(Vector2f position, Vector2f size, float moveSpeed, float range, const Texture& texture, int skinIndex);
    void update(float deltaTime) override;
};
