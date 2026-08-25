#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <memory>

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

    void superJump() {
       velocity.y = -1200.f;
    }

    void update(float deltaTime, float gravity) {
        velocity.y += gravity * deltaTime;
        position.y += velocity.y * deltaTime;
    }

    void handleInput(float deltaTime, Sprite& player) {
        float distance = speed * deltaTime;

        if (Keyboard::isKeyPressed(Scan::A)) {
            position.x -= distance;
            player.setScale({ -1.5f, 1.5f });
        }
        if (Keyboard::isKeyPressed(Scan::D)) {
            position.x += distance;
            player.setScale({ 1.5f,1.5f });
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

    void landOn(float platformTopY) {
        position.y = platformTopY - size.y / 2.f;
        jump();
    }

    void reset(Vector2f startPosition) {
        position = startPosition;
        velocity = { 0.f,0.f };
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
protected:
    RectangleShape shape;

public:
    Platform(Vector2f position, Vector2f size) {
        shape.setSize(size);
        shape.setFillColor(Color::White);
        shape.setOrigin(shape.getGeometricCenter());
        shape.setPosition(position);
    }

    virtual void draw(RenderWindow& window) const {
        window.draw(shape);
    }

    virtual ~Platform() {}

    virtual void update(float deltaTime) {}

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

class MovingPlatform : public Platform {
    float speed;
    float leftBound;
    float rightBound;
    int direction = 1;
public:
    MovingPlatform(Vector2f position, Vector2f size, float moveSpeed, float range)
        : Platform(position, size), speed(moveSpeed),
        leftBound(position.x - range), rightBound(position.x + range) {
        shape.setFillColor(Color::Cyan);
    }

    void update(float deltaTime) override{
        float dx = speed * direction * deltaTime;
        shape.move({ dx,0.f });

        if (shape.getPosition().x <= leftBound || shape.getPosition().x >= rightBound || shape.getPosition().x - 50.f <= 0.f || shape.getPosition().x + 50.f >= 600.f)
        {
            direction *= -1;
        }
    }
};

class BreakablePlatform : public Platform {
    bool isBroken = false;
public:
    BreakablePlatform(Vector2f position, Vector2f size)
        : Platform(position, size)
    {
        shape.setFillColor(Color::Red);
    }

    void breakPlatform() {
        isBroken = true;
    }

    bool isGetBroken() const{
        return isBroken;
    }

    void draw(RenderWindow& window) const override {
        if (!isBroken) {
            window.draw(shape);
        }
    }
};

class PowerUp {
    Platform* attachedPlatform;
    RectangleShape shape;
    bool isCollected = false;
public:
    PowerUp(Vector2f position, Vector2f size, Platform* attachedPlatformptr) {
        attachedPlatform = attachedPlatformptr;
        shape.setSize(size);
        shape.setFillColor(Color::Magenta);
        shape.setOrigin(shape.getGeometricCenter());
        shape.setPosition(position);
    }

    void draw(RenderWindow& window) {
        if (isCollected == false)
        {
            window.draw(shape);
        }
    }

    void collectPower() {
        isCollected = true;
    }

    void update() {
        shape.setPosition({ attachedPlatform->getPosition().x, attachedPlatform->getPosition().y - 20.f});
    }

    Vector2f getAttachedPlatformPostion() {
        return attachedPlatform->getPosition();
    }

    FloatRect getBounds() const {
        return shape.getGlobalBounds();
    }

    bool isGetCollected() const {
        return isCollected;
    }

    Vector2f getPosition() const {
        return shape.getPosition();
    }

    Vector2f getSize() const {
        return shape.getSize();
    }
};

enum class GameState {
    Menu,
    Playing,
    GameOver
};

enum class TextureState {
    Idle,
    Jump,
    Fall
};


void resetGame(Player& player, vector<unique_ptr<Platform>>& platforms,
    vector<PowerUp>& powerUps, float& highestPoint,
    float& lastPlatformY, View& camera)
{
    player.reset({ 100.f, 600.f });

    platforms.clear();
    powerUps.clear();

    float spacing = 200.f;
    float startY = 750.f;
    for (int i = 0; i < 4; ++i)
    {
        float x = 100.f + (i % 2) * 400.f;
        float y = startY - (i * spacing);
        platforms.push_back(make_unique<Platform>(Vector2f{ x, y }, Vector2f{ 100.f, 20.f }));
    }

    highestPoint = 750.f;
    lastPlatformY = startY - (4 * spacing) + 100.f;
    camera.setCenter({ 300.f, highestPoint + 100.f });

}

int main()
{
    mt19937 rng(random_device{}());
    uniform_real_distribution<float> distX(50.f, 550.f);
    uniform_int_distribution<int> randPlatform(1, 3);
    uniform_int_distribution<int> chance(1, 100);

    RenderWindow window(VideoMode({ 600, 800 }), "SFML works!");

    window.setFramerateLimit(60);

    float XSizeRect = 50.f;
    float YSizeRect = 50.f;

    Texture playerTexture;
    playerTexture.loadFromFile("Textures/Idle.png");

    Texture jumpTexture;
    jumpTexture.loadFromFile("Textures/Jump2.png");

    Texture fallTexture;
    fallTexture.loadFromFile("Textures/Fall.png");

    Texture bgTexture;
    bgTexture.loadFromFile("Textures/bg.png");

    Sprite playerSprite(playerTexture);
    playerSprite.setTextureRect({ {0,0},{128,128} });
    playerSprite.setOrigin({ 64.f,64.f });
    playerSprite.setScale({ 1.5f,1.5f });

    Sprite backgroundSprite(bgTexture);
    backgroundSprite.setScale({ 3.f,3.f });
    
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

    Text resetText(font);
    resetText.setCharacterSize(30);
    resetText.setFillColor(Color::White);
    resetText.setPosition({ 10.f, 40.f });

    Clock clock;
    Clock animationClock;
    int currentFrame = 0;

    Player player({ 100.f, 600.f }, { 50.f, 50.f }, 200.f);

    View camera({ FloatRect({0.f, 0.f}, {600.f, 800.f}) });
    View uiView({ FloatRect({0.f, 0.f}, {600.f, 800.f}) });

    vector<unique_ptr<Platform>> platforms;
    vector<PowerUp> powerUps;
    float platformCount = 8;
    float startY = 750.f;
    float spacing = 200.f;
    float highestPoint = 750.f;
    float lastPlatformY = startY - (4 * spacing) + 100.f;
    bool isJump = false;
    int score = static_cast<int>(startY - highestPoint);

    GameState currentState = GameState::Menu;
    TextureState playerState = TextureState::Idle;

    for (int i = 0; i < 4; ++i)
    {
        float x = 100.f + (i % 2) * 400.f;
        float y = startY - (i * spacing);
        platforms.push_back(make_unique<Platform>(Vector2f{ x, y }, Vector2f{ 100.f, 20.f }));
    }

    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        resetText.setString("");

        while (const optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
                window.close();

            if (currentState == GameState::Menu)
            {
                if (const auto* keyPressed = event->getIf<Event::KeyPressed>())
                {
                    if (keyPressed->scancode == Scan::Space)
                    {
                        currentState = GameState::Playing;
                    }
                }
            }

            if (currentState == GameState::GameOver)
            {
                if (const auto* keyPressed = event->getIf<Event::KeyPressed>())
                {
                    if (keyPressed->scancode == Scan::Space)
                    {
                        resetGame(player,platforms,powerUps,highestPoint,lastPlatformY,camera);
                        currentState = GameState::Playing;
                    }
                }
            }
        }

        if (currentState == GameState::Menu)
        {
            scoreText.setString("Press SPACE to start");
        }
        else if (currentState == GameState::Playing)
        {
            score = static_cast<int>(startY - highestPoint);

            scoreText.setString("Score: " + to_string(score));
            if (highestPoint < lastPlatformY + 100.f) {
                lastPlatformY -= spacing + 25.f;
                float x = distX(rng);
                int platformType = randPlatform(rng);
                int powerUpChance = chance(rng);

                if (platformType == 1)
                {
                    platforms.push_back(make_unique<Platform>(Vector2f{ x, lastPlatformY }, Vector2f{ 100.f, 20.f }));
                }
                else if (platformType == 2) {
                    platforms.push_back(make_unique<MovingPlatform>(Vector2f{ x, lastPlatformY }, Vector2f{ 100.f, 20.f }, 75.f, 200.f));
                }
                else {
                    platforms.push_back(make_unique<BreakablePlatform>(Vector2f{ x, lastPlatformY }, Vector2f{ 100.f, 20.f }));
                }

                if (powerUpChance <= 20 && powerUpChance >= 1)
                {
                    powerUps.push_back(PowerUp(Vector2f{ x, lastPlatformY - 20.f }, Vector2f{ 50.f,20.f }, platforms.back().get()));
                }
            }

            powerUps.erase(remove_if(powerUps.begin(), powerUps.end(),
                [&highestPoint](PowerUp& powerUp) {
                    return powerUp.getAttachedPlatformPostion().y > highestPoint + 400.f;
                }), powerUps.end());

            platforms.erase(remove_if(platforms.begin(), platforms.end(),
                [&highestPoint](const unique_ptr<Platform>& platform) {
                    return platform->getPosition().y > highestPoint + 400.f;
                }), platforms.end());
            

            player.handleInput(deltaTime, playerSprite);
            player.update(deltaTime, gravity);
            player.barrierX(600.f, player.getSize().x);
            playerSprite.setPosition(player.getPosition());

            if (player.getPosition().y < highestPoint)
            {
                highestPoint = player.getPosition().y;
            }

            camera.setCenter({ 300.f, highestPoint + 100.f });

            if (player.getPosition().y > highestPoint + 600.f)
            {
                currentState = GameState::GameOver;
            }

            if (player.getVelocity().y > 0.f)
            {
                playerState = TextureState::Fall;
            }
            else if (player.getVelocity().y < 0.f) {
                playerState = TextureState::Jump;
            }
            else {
                playerState = TextureState::Idle;
            }

            if (animationClock.getElapsedTime().asSeconds() > 0.1f)
            {
                currentFrame++;

                if (playerState == TextureState::Idle)
                {
                    playerSprite.setTexture(playerTexture);
                    if (currentFrame >= 10)
                        currentFrame = 0;

                    playerSprite.setTextureRect(
                        IntRect({ currentFrame * 128, 0 }, { 128, 128 })
                    );

                    animationClock.restart();
                    isJump = false;
                }
                else if (playerState == TextureState::Fall) {
                    playerSprite.setTexture(fallTexture);
                    if (currentFrame >= 4)
                        currentFrame = 0;

                    playerSprite.setTextureRect(
                        IntRect({ currentFrame * 128, 0 }, { 128, 128 })
                    );

                    animationClock.restart();
                    isJump = false;
                }
                else {
                    if (!isJump)
                    {
                        currentFrame = 0;
                        isJump = true;
                    }
                    playerSprite.setTexture(jumpTexture);
                    if (currentFrame >= 2)
                        currentFrame = 1;

                    playerSprite.setTextureRect(
                        IntRect({ currentFrame * 128, 0 }, { 128, 128 })
                    );

                    animationClock.restart();
                }

            }

        }
        else if (currentState == GameState::GameOver)
        {
            scoreText.setString("GAME OVER! Score: " + to_string(score));
            resetText.setString("Press SPACE to restart");

        }
        backgroundSprite.setPosition({ camera.getCenter().x - 300.f, camera.getCenter().y - 400.f});

        window.clear();
        window.setView(camera);
        window.draw(backgroundSprite);
        for (const unique_ptr<Platform>& platform : platforms) {
            platform->draw(window);
            platform->update(deltaTime);

            bool isFalling = player.getVelocity().y > 0.f;
            auto intersection = player.getBounds().findIntersection(platform->getBounds());
            if (isFalling && intersection.has_value())
            {
                float playerBottom = player.getPosition().y + player.getSize().y / 2.f;
                float platformTop = platform->getPosition().y - platform->getSize().y / 2.f;

                float tolerance = platform->getSize().y - 3.f;
                if (playerBottom - platformTop <= tolerance)
                {
                    BreakablePlatform* breakable = dynamic_cast<BreakablePlatform*>(platform.get());
                    if (breakable == nullptr || !breakable->isGetBroken())
                    {
                        player.landOn(platformTop);

                    }
                    if (breakable != nullptr)
                    {
                        breakable->breakPlatform();
                    }
                }
            }
        }

        for (PowerUp& powerUp : powerUps) {
            if (!powerUp.isGetCollected())
            {
                powerUp.draw(window);
                powerUp.update();
                auto intersection = player.getBounds().findIntersection(powerUp.getBounds());
                if (intersection.has_value())
                {
                    player.superJump();
                    powerUp.collectPower();
                }
            }
        }
        window.draw(playerSprite);
        window.setView(uiView);
        window.draw(scoreText);
        window.draw(resetText);
        window.display();

    }

}