#include "Player.h"

using namespace sf;

Player::Player(Vector2f startPosition, Vector2f playerSize, float moveSpeed)
    : position(startPosition), velocity(0.f, 0.f),
      size(playerSize), speed(moveSpeed)
{
}

void Player::jump()
{
    velocity.y = -800.f;
}

void Player::superJump()
{
    velocity.y = -1200.f;
}

void Player::update(float deltaTime, float gravity)
{
    velocity.y += gravity * deltaTime;
    position.y += velocity.y * deltaTime;
}

void Player::handleInput(float deltaTime, Sprite& player)
{
    float distance = speed * deltaTime;

    if (Keyboard::isKeyPressed(Keyboard::Scan::A))
    {
        position.x -= distance;
        player.setScale({ -1.5f, 1.5f });
    }

    if (Keyboard::isKeyPressed(Keyboard::Scan::D))
    {
        position.x += distance;
        player.setScale({ 1.5f, 1.5f });
    }
}

FloatRect Player::getBounds() const
{
    return FloatRect(
        { position.x - size.x / 2.f, position.y - size.y / 2.f },
        size
    );
}

void Player::wrapX(float windowWidth)
{
    float halfWidth = size.x / 2.f;

    if (position.x < -halfWidth)
    {
        position.x = windowWidth + halfWidth;
    }

    if (position.x > windowWidth + halfWidth)
    {
        position.x = -halfWidth;
    }
}

void Player::landOn(float platformTopY)
{
    position.y = platformTopY - size.y / 2.f;
    jump();
}

void Player::reset(Vector2f startPosition)
{
    position = startPosition;
    velocity = { 0.f, 0.f };
}

Vector2f Player::getPosition() const
{
    return position;
}

Vector2f Player::getVelocity() const
{
    return velocity;
}

Vector2f Player::getSize() const
{
    return size;
}
