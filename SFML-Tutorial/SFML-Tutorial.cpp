#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
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

class Player {
    Vector2f position;
    Vector2f velocity;
    Vector2f size;
    float speed = 250.f;

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
    Platform(Vector2f position, Vector2f size, const Texture& texture, int skinIndex) {
        shape.setSize(size);
        shape.setTexture(&texture);
        shape.setTextureRect(IntRect({ 0, skinIndex*16 }, { 33, 16 }));
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
    MovingPlatform(Vector2f position, Vector2f size, float moveSpeed, float range, const Texture& texture, int skinIndex)
        : Platform(position, size, texture, skinIndex), speed(moveSpeed),
        leftBound(position.x - range), rightBound(position.x + range) {
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
    BreakablePlatform(Vector2f position, Vector2f size, const Texture& texture, int skinIndex)
        : Platform(position, size, texture, skinIndex)
    {
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
    PowerUp(Vector2f position, Vector2f size, Platform* attachedPlatformptr, const Texture& texture) {
        attachedPlatform = attachedPlatformptr;
        shape.setSize(size);
        shape.setTexture(&texture);
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
        shape.setPosition({ attachedPlatform->getPosition().x, attachedPlatform->getPosition().y - 34.f});
    }

    bool isPlatformBroken() const {
        BreakablePlatform* breakable =
            dynamic_cast<BreakablePlatform*>(attachedPlatform);

        return breakable != nullptr && breakable->isGetBroken();
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

class Bird {
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
    Bird(Vector2f position, Vector2f size, float moveSpeed, float range, const Texture& texture) {
        shape.setSize(size);
        shape.setTexture(&texture);
        shape.setTextureRect(IntRect({ 0, 0 }, { FRAME_WIDTH, FRAME_HEIGHT }));
        shape.setOrigin(shape.getGeometricCenter());
        shape.setPosition(position);
        speed = moveSpeed;
        leftBound = max(position.x - range, 60.f);
        rightBound = min(position.x + range, 540.f);
    }

    void update(float deltaTime) {
        if (isKilled) return;

        float dx = speed * direction * deltaTime;
        shape.move({ dx, 0.f });

        if (shape.getPosition().x <= leftBound || shape.getPosition().x >= rightBound)
        {
            direction *= -1;
        }

        if(direction == 1) {
            shape.setScale({ -1.75f,1.75f });
        }
        if (direction == -1) {
            shape.setScale({ 1.75f,1.75f });
        }

        if (animationClock.getElapsedTime().asSeconds() > 0.1f)
        {
            currentFrame++;
            if (currentFrame >= FRAME_COUNT)
                currentFrame = 0;

            shape.setTextureRect(IntRect({ currentFrame * 64, 0 }, { FRAME_WIDTH, FRAME_HEIGHT }));
            animationClock.restart();
        }
    }

    void draw(RenderWindow& window) {
        if (!isKilled)
        {
            window.draw(shape);
        }
    }

    void kill() {
        isKilled = true;
    }

    bool isDead() const {
        return isKilled;
    }

    Vector2f getPosition() const {
        return shape.getPosition();
    }

    Vector2f getSize() const {
        return shape.getSize();
    }

    FloatRect getBounds() const {
        FloatRect bounds = shape.getGlobalBounds();

        float leftInset;
        float rightInset;

        if (direction == -1) {
            leftInset = 14.f;
            rightInset = 18.f;
        }
        else {
            leftInset = 18.f;
            rightInset = 14.f;
        }

        return FloatRect(
            { bounds.position.x + leftInset, bounds.position.y + 9.f },
            { bounds.size.x - leftInset - rightInset, bounds.size.y - 18.f }
        );
    }
};

void resetGame(Player& player, vector<unique_ptr<Platform>>& platforms,
    vector<PowerUp>& powerUps, vector<Bird>& birds, float& highestPoint,
    float& lastPlatformY, View& camera, Texture& platformTexture)
{
    player.reset({ 100.f, 600.f });

    platforms.clear();
    powerUps.clear();
    birds.clear();

    float spacing = 200.f;
    float startY = 750.f;
    for (int i = 0; i < 4; ++i)
    {
        float x = 100.f + (i % 2) * 300.f;
        float y = startY - (i * spacing);
        platforms.push_back(make_unique<Platform>(Vector2f{ x, y }, Vector2f{ 100.f, 40.f }, platformTexture, 0));
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

    RenderWindow window(VideoMode({ 600, 800 }), "SFML works!", Style::Titlebar | Style::Close);

    window.setFramerateLimit(60);

    float XSizeRect = 50.f;
    float YSizeRect = 50.f;

    SoundBuffer jumpBuffer;
    jumpBuffer.loadFromFile("Sounds/jump.mp3");
    Sound jumpSound(jumpBuffer);

    SoundBuffer gameOverBuffer;
    gameOverBuffer.loadFromFile("Sounds/gameover.mp3");
    Sound gameOverSound(gameOverBuffer);

    Texture platformTexture;
    platformTexture.loadFromFile("Textures/platforms2.png");

    Texture playerTexture;
    playerTexture.loadFromFile("Textures/Idle.png");

    Texture jumpTexture;
    jumpTexture.loadFromFile("Textures/Jump2.png");

    Texture fallTexture;
    fallTexture.loadFromFile("Textures/Fall.png");

    Texture bgTexture;
    bgTexture.loadFromFile("Textures/bg.png");

    Texture powerUpTexture;
    powerUpTexture.loadFromFile("Textures/Bonus.png");

    Texture birdTexture;
    birdTexture.loadFromFile("Textures/Bird.png");

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

    Text titleText(font);
    titleText.setCharacterSize(60);
    titleText.setFillColor(Color::White);
    titleText.setString("DOODLE JUMP");
    titleText.setPosition({ 50.f, 200.f });

    Text startText(font);
    startText.setCharacterSize(30);
    startText.setFillColor(Color::White);
    startText.setString("Press SPACE to start");
    startText.setPosition({ 100.f, 400.f });

    Clock clock;
    Clock animationClock;
    int currentFrame = 0;

    Player player({ 100.f, 600.f }, { 10.f, 50.f }, 200.f);

    View camera({ FloatRect({0.f, 0.f}, {600.f, 800.f}) });
    View uiView({ FloatRect({0.f, 0.f}, {600.f, 800.f}) });

    vector<unique_ptr<Platform>> platforms;
    vector<PowerUp> powerUps;
    vector<Bird> birds;
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
        float x = 100.f + (i % 2) * 300.f;
        float y = startY - (i * spacing);
        platforms.push_back(make_unique<Platform>(Vector2f{ x, y }, Vector2f{ 100.f, 40.f }, platformTexture, 0));
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
                        resetGame(player, platforms, powerUps, birds,highestPoint, lastPlatformY, camera, platformTexture);
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
                int birdChance = chance(rng);

                if (platformType == 1)
                {
                    platforms.push_back(make_unique<Platform>(Vector2f{ x, lastPlatformY }, Vector2f{ 100.f, 40.f }, platformTexture, 0));
                }
                else if (platformType == 2) {
                    platforms.push_back(make_unique<MovingPlatform>(Vector2f{ x, lastPlatformY }, Vector2f{ 100.f, 40.f }, 75.f, 200.f, platformTexture, 3));
                }
                else {
                    platforms.push_back(make_unique<BreakablePlatform>(Vector2f{ x, lastPlatformY }, Vector2f{ 100.f, 40.f }, platformTexture, 1));
                }

                if (powerUpChance <= 15 && powerUpChance >= 1)
                {
                    powerUps.push_back(PowerUp(Vector2f{ x, lastPlatformY - 10.f }, Vector2f{ 50.f,50.f }, platforms.back().get(), powerUpTexture));
                }

                if (birdChance <= 15 && birdChance >= 1)
                {
                    birds.push_back(Bird(Vector2f{ x, lastPlatformY - 150.f }, Vector2f{ 64.f, 54.f }, 100.f, 150.f, birdTexture));
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

            birds.erase(remove_if(birds.begin(), birds.end(),
                [&highestPoint](Bird& bird) {
                    return bird.getPosition().y > highestPoint + 400.f;
                }), birds.end());
            

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
                gameOverSound.play();

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

        if (currentState == GameState::Playing || currentState == GameState::GameOver)
        {
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
                            jumpSound.play();
                        }
                        if (breakable != nullptr)
                        {
                            breakable->breakPlatform();
                        }
                    }
                }
            }
            for (PowerUp& powerUp : powerUps) {
                if (!powerUp.isGetCollected() && !powerUp.isPlatformBroken())
                {
                    powerUp.draw(window);
                    powerUp.update();

                    bool isFalling = player.getVelocity().y > 0.f;
                    auto intersection =
                        player.getBounds().findIntersection(powerUp.getBounds());

                    if (isFalling && intersection.has_value())
                    {
                        player.superJump();
                        jumpSound.play();
                        powerUp.collectPower();
                    }
                }
            }
            window.draw(playerSprite);

            for (Bird& bird : birds) {
                if (!bird.isDead())
                {
                    bird.draw(window);
                    bird.update(deltaTime);

                    bool isFalling = player.getVelocity().y > 0.f;
                    auto intersection = player.getBounds().findIntersection(bird.getBounds());

                    if (intersection.has_value())
                    {
                        float playerBottom = player.getPosition().y + player.getSize().y / 2.f;
                        float birdTop = bird.getPosition().y - bird.getSize().y / 2.f;
                        float tolerance = bird.getSize().y - 3.f;

                        if (isFalling && (playerBottom - birdTop <= tolerance))
                        {
                            bird.kill();
                            player.jump();
                            jumpSound.play();
                        }
                        else
                        {
                            currentState = GameState::GameOver;
                            gameOverSound.play();

                        }

                    }
                }
            }
        }

        window.setView(uiView);
        if (currentState == GameState::Menu)
        {
            window.draw(titleText);
            window.draw(startText);
        }
        else
        {
            window.draw(scoreText);
            window.draw(resetText);
        }
        window.display();

    }

}