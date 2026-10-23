#pragma once
#include "Platform.h"

using namespace sf;

class BreakablePlatform : public Platform {
private:
    bool isBroken = false;

public:
    BreakablePlatform(Vector2f position, Vector2f size, const Texture& texture, int skinIndex);

    void breakPlatform();

    bool isGetBroken() const;

    void draw(RenderWindow& window) const override;
};
