#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <iostream>
#include <random>
#include <algorithm>
#include <string>

using namespace sf;
using namespace std;

using Keyboard::isKeyPressed;
using Keyboard::Scan;

class Player {
    Vector2f position;
    Vector2f velocity;
    Vector2f size;
    float speed = 200.f;

public:
    Player(Vector2f startPosition, Vector2f playerSize, float moveSpeed)
        : position(startPosition), velocity(0.f, 0.f),
        size(playerSize), speed(moveSpeed)
    {
    }

    void jump() {
        velocity.y = -800.f;
    }

    void update(float deltaTime, float gravity) {
        velocity.y += gravity * deltaTime;
        position.y += velocity.y * deltaTime;
    }

    void handleInput(float deltaTime) {
        float distance = speed * deltaTime;

        if (Keyboard::isKeyPressed(Scan::A)) {
            position.x -= distance;
        }
        if (Keyboard::isKeyPressed(Scan::D)) {
            position.x += distance;
        }
    }

    FloatRect getBounds() const {
        return FloatRect(
            { position.x - size.x / 2.f, position.y - size.y / 2.f },
            size
        );
    }

    void barrierX(float windowWidth, float shapeWidth) {
        float halfWidth = shapeWidth / 2.f;

        if (position.x < halfWidth) position.x = halfWidth;
        if (position.x > windowWidth - halfWidth) position.x = windowWidth - halfWidth;
    }

    void groundCheck(float ground) {
        if (position.y > ground)
        {
            position.y = ground;
            jump();
        }
    }

    void landOn(float platformTopY) {
        position.y = platformTopY - size.y / 2.f;
        jump();
    }

    Vector2f getPosition() const {
        return position;
    }

    Vector2f getVelocity() const {
        return velocity;
    }

    Vector2f getSize() const {
        return size;
    }
};

class Platform {
    RectangleShape shape;

public:
    Platform(Vector2f position, Vector2f size) {
        shape.setSize(size);
        shape.setFillColor(Color::White);
        shape.setOrigin(shape.getGeometricCenter());
        shape.setPosition(position);
    }

    void draw(RenderWindow& window) const {
        window.draw(shape);
    }

    Vector2f getPosition() const {
        return shape.getPosition();
    }

    Vector2f getSize() const {
        return shape.getSize();
    }

    FloatRect getBounds() const {
        return shape.getGlobalBounds();
    }

};


int main()
{
    mt19937 rng(random_device{}());
    uniform_real_distribution<float> distX(50.f, 550.f);

    RenderWindow window(VideoMode({ 600, 800 }), "SFML works!");

    window.setFramerateLimit(60);

    float XSizeRect = 50.f;
    float YSizeRect = 50.f;

    RectangleShape shape({ XSizeRect, YSizeRect });
    shape.setOrigin(shape.getGeometricCenter());
    shape.setPosition(window.getView().getSize() / 2.f);

    float ground = 800.f - YSizeRect / 2.f;
    float gravity = 900.f;

    Font font;
    if (!font.openFromFile("Roboto-Italic-VariableFont_wdth,wght.ttf"))
    {
        cout << "Failed to load font!" << endl;
    }

    Text scoreText(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition({ 10.f, 10.f });

    Clock clock;

    Player player({ 100.f, 600.f }, { 50.f, 50.f }, 200.f);

    View camera({ FloatRect({0.f, 0.f}, {600.f, 800.f}) });
    View uiView({ FloatRect({0.f, 0.f}, {600.f, 800.f}) });

    vector<Platform> platforms;
    float platformCount = 8;
    float startY = 750.f;
    float spacing = 200.f;
    float highestPoint = 750.f;
    float lastPlatformY = startY - (4 * spacing) + 100.f;
    int score = static_cast<int>(startY - highestPoint);

    bool isGameOver = false;

    for (int i = 0; i < 4; ++i)
    {
        float x = 100.f + (i % 2) * 400.f;
        float y = startY - (i * spacing);
        platforms.push_back(Platform({ x,y }, { 100.f, 20.f }));
    }

    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();

        while (const optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
                window.close();
        }

        if (!isGameOver)
        {
            score = static_cast<int>(startY - highestPoint);

            scoreText.setString("Score: " + to_string(score));
            if (highestPoint < lastPlatformY + 100.f) {
                lastPlatformY -= spacing + 25.f;
                float x = distX(rng);
                platforms.push_back(Platform({ x, lastPlatformY }, { 100.f, 20.f }));
            }

            platforms.erase(remove_if(platforms.begin(), platforms.end(),
                [&highestPoint](const Platform& platform) {
                    return platform.getPosition().y > highestPoint + 400.f;
                }), platforms.end());

            

            player.handleInput(deltaTime);
            player.update(deltaTime, gravity);
            player.groundCheck(ground);
            player.barrierX(600.f, 50.f);
            shape.setPosition(player.getPosition());

            if (player.getPosition().y < highestPoint)
            {
                highestPoint = player.getPosition().y;
            }

            camera.setCenter({ 300.f, highestPoint + 100.f });

            if (player.getPosition().y > highestPoint + 400.f)
            {
                isGameOver = true;
            }
        }

        if (isGameOver)
        {
            scoreText.setString("GAME OVER! Score: " + to_string(score));
        }

        window.clear();
        window.setView(camera);
        window.draw(shape);
        for (const Platform& platform : platforms) {
            platform.draw(window);

            bool isFalling = player.getVelocity().y > 0.f;
            auto intersection = player.getBounds().findIntersection(platform.getBounds());

            if (isFalling && intersection.has_value())
            {
                float playerBottom = player.getPosition().y + player.getSize().y / 2.f;
                float platformTop = platform.getPosition().y - platform.getSize().y / 2.f;

                float tolerance = platform.getSize().y - 3.f;
                if (playerBottom - platformTop <= tolerance)
                {
                    player.landOn(platformTop);
                }
            }
        }
        window.setView(uiView);
        window.draw(scoreText);
        window.display();

    }

}