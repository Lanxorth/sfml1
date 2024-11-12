#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>             

// Structures de base pour ECS
struct Transform2D {
    sf::Vector2f position;
    sf::Vector2f size;
};

struct Velocity {
    sf::Vector2f speed;
};

struct RenderComponent {
    sf::Drawable* drawable;

    // Constructor for sf::Shape-based renderables (like sf::RectangleShape)
    RenderComponent(sf::Shape* shape) : drawable(shape) {}

    // Constructor for sf::Sprite-based renderables
    RenderComponent(sf::Sprite* sprite) : drawable(sprite) {}

    // Getter function to access the drawable object
    sf::Drawable* getDrawable() const {
        return drawable;
    }
};

struct Behavior {
    enum Type { Ball, Platform, Brick } type;
};



// Fonctions des systèmes
void renderSystem1(sf::RenderWindow& window, const std::vector<RenderComponent>& renderables) {
    for (const auto& renderable : renderables) {
        window.draw(*renderable.drawable);
    }
}

void createEntityBall(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors, const sf::Texture& textureBall) {

    // Create a ball
    Transform2D ballTransform{ sf::Vector2f(957.f,900.f), sf::Vector2f(40.f, 40.f) };
    Velocity ballVelocity{ sf::Vector2f(400.f, -400.f) };
    Behavior ballBehavior{ Behavior::Ball };
    sf::Sprite* spriteBall = new sf::Sprite();
    spriteBall->setTexture(textureBall);
    spriteBall->setPosition(975.f, 925.f);
    RenderComponent ballRender{ spriteBall };

    transforms.push_back(ballTransform);
    velocities.push_back(ballVelocity);
    renderables.push_back(ballRender);
    behaviors.push_back(ballBehavior);

}

void createEntityPlatform(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors, const sf::Texture& texturePlatform) {

    // Create a platform
    Transform2D platformTransform{ sf::Vector2f(900.f, 950.f), sf::Vector2f(150.f, 30.f) };
    Velocity platformVelocity{ sf::Vector2f(600.f, 0) };
    Behavior platformBehavior{ Behavior::Platform };
    sf::Sprite* spritePlatform = new sf::Sprite();
    spritePlatform->setTexture(texturePlatform);
    spritePlatform->setPosition(900.f, 950.f);
    RenderComponent platformRender{ spritePlatform };


    transforms.push_back(platformTransform);
    velocities.push_back(platformVelocity);
    renderables.push_back(platformRender);
    behaviors.push_back(platformBehavior);

}

void removePlayer(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors) {
    // Find the index of the ball (based on the Behavior type being Ball)
    for (size_t i = 0; i < behaviors.size(); ++i) {
        if (behaviors[i].type == Behavior::Ball) {
            // Remove the ball from all ECS components
            transforms.erase(transforms.begin() + i);
            velocities.erase(velocities.begin() + i);
            renderables.erase(renderables.begin() + i);
            behaviors.erase(behaviors.begin() + i);
        }
    }
    for (size_t i = 0; i < behaviors.size(); ++i) {
        if (behaviors[i].type == Behavior::Platform) {
            // Remove the platform from all ECS components
            transforms.erase(transforms.begin() + i);
            velocities.erase(velocities.begin() + i);
            renderables.erase(renderables.begin() + i);
            behaviors.erase(behaviors.begin() + i);
        }
    }
}

void createEntityBrick(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors,
    float x, float y, float brickWidth, float brickHeight, const sf::Texture& textureBrick) {

    // Create a brick
    Transform2D brickTransform{ sf::Vector2f(x, y), sf::Vector2f(brickWidth, brickHeight) };
    Velocity brickVelocity{ sf::Vector2f(0, 0) };
    Behavior brickBehavior{ Behavior::Brick };
    sf::Sprite* spriteBrick = new sf::Sprite();
    spriteBrick->setTexture(textureBrick);
    spriteBrick->setPosition(x, y);
    spriteBrick->setScale(brickWidth / spriteBrick->getLocalBounds().width,
        brickHeight / spriteBrick->getLocalBounds().height);
    RenderComponent brickRender{ spriteBrick };

    transforms.push_back(brickTransform);
    velocities.push_back(brickVelocity);
    renderables.push_back(brickRender);
    behaviors.push_back(brickBehavior);

}

void removeBrick(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors, size_t brickIndex) {

    // Remove the brick from all ECS components
    transforms.erase(transforms.begin() + brickIndex);
    velocities.erase(velocities.begin() + brickIndex);
    renderables.erase(renderables.begin() + brickIndex);
    behaviors.erase(behaviors.begin() + brickIndex);
}

void createEntitiesBricksGrid(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors,
    float startX, float startY, int rows, int cols, float spacing, float brickWidth, float brickHeight, const sf::Texture& textureBrick) {

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            float x = startX + col * (100.0f + spacing); // Fixed width
            float y = startY + row * (20.0f + spacing);  // Fixed height
            createEntityBrick(transforms, velocities, renderables, behaviors, x, y, brickWidth, brickHeight, textureBrick);
        }
    }
}

// Function to check for collision between ball and brick
bool checkCollision(const Transform2D& ball, const Transform2D& brick) {
    return ball.position.x < brick.position.x + brick.size.x &&
        ball.position.x + ball.size.x > brick.position.x &&
        ball.position.y < brick.position.y + brick.size.y &&
        ball.position.y + ball.size.y > brick.position.y;
}

void handleBallBrickCollision(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors, size_t ballIndex, size_t brickIndex) {

    // Reverse ball velocity upon collision with a brick
    Transform2D& ballTransform = transforms[ballIndex];
    Velocity& ballVelocity = velocities[ballIndex];

    Transform2D& brickTransform = transforms[brickIndex];

    // Check which side of the brick the ball hit
    float ballCenterX = ballTransform.position.x + ballTransform.size.x / 2;
    float ballCenterY = ballTransform.position.y + ballTransform.size.y / 2;

    float brickCenterX = brickTransform.position.x + brickTransform.size.x / 2;
    float brickCenterY = brickTransform.position.y + brickTransform.size.y / 2;

    float deltaX = ballCenterX - brickCenterX;
    float deltaY = ballCenterY - brickCenterY;

    float overlapX = ballTransform.size.x / 2 + brickTransform.size.x / 2 - std::abs(deltaX);
    float overlapY = ballTransform.size.y / 2 + brickTransform.size.y / 2 - std::abs(deltaY);

    if (overlapX > 0 && overlapY > 0) {
        if (overlapX < overlapY) {
            ballVelocity.speed.x = -ballVelocity.speed.x; // Ball hits the side
        }
        else {
            ballVelocity.speed.y = -ballVelocity.speed.y; // Ball hits the top or bottom
        }
    }

    // Mark the brick for removal (for now, just deactivate its rendering)
    removeBrick(transforms, velocities, renderables, behaviors, brickIndex);
}

int main() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Casse-Brique", sf::Style::Fullscreen);
    sf::Clock clock;

    // Load background texture
    sf::Texture texture, textureVictoire, textureBrick, textureBall, texturePlatform;
    if (!texture.loadFromFile("background.jpg") ||
        !textureVictoire.loadFromFile("Victoire.png") ||
        !textureBrick.loadFromFile("Brick2.png") ||
        !textureBall.loadFromFile("ball.png") ||
        !texturePlatform.loadFromFile("paddle.png")) {
        return 0;
    }

    sf::Sprite sprite;
    sprite.setTexture(texture);

    // ECS Components
    std::vector<Transform2D> transforms;
    std::vector<Velocity> velocities;
    std::vector<RenderComponent> renderables;
    std::vector<Behavior> behaviors;


    // Define brick dimensions based on window size
    float brickWidth = window.getSize().y * 0.095f;
    float brickHeight = window.getSize().x * 0.014f;

    createEntityBall(transforms, velocities, renderables, behaviors, textureBall);
    createEntityPlatform(transforms, velocities, renderables, behaviors, texturePlatform);
    bool platformMoved = false;


    // Create a grid of bricks with fixed size
    createEntitiesBricksGrid(transforms, velocities, renderables, behaviors, 100, 100, 10, 16, 10, brickWidth, brickHeight, textureBrick);
    bool gameWon = false;
    bool Won = false;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        // Event handling
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                window.close();
            }
        }

        // Platform movement
        for (size_t i = 0; i < behaviors.size(); ++i) {
            if (behaviors[i].type == Behavior::Platform) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                    transforms[i].position.x -= velocities[i].speed.x * deltaTime;
                    platformMoved = true;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                    transforms[i].position.x += velocities[i].speed.x * deltaTime;
                    platformMoved = true;
                }

                // Ensure the platform stays within the window bounds
                if (transforms[i].position.x < 0) transforms[i].position.x = 0;
                if (transforms[i].position.x + transforms[i].size.x > window.getSize().x) {
                    transforms[i].position.x = window.getSize().x - transforms[i].size.x;
                }
                //reset platform
                if (transforms[i].position.y > window.getSize().y) {
                    transforms[i].position = sf::Vector2f(900.f, 950.f);
                }
            }
        }

        // Ball movement and collision
        for (size_t i = 0; i < behaviors.size(); ++i) {
            if (behaviors[i].type == Behavior::Ball && platformMoved) {
                transforms[i].position.x += velocities[i].speed.x * deltaTime;
                transforms[i].position.y += velocities[i].speed.y * deltaTime;

                // Ball collision with walls
                if (transforms[i].position.x <= 0 || transforms[i].position.x + transforms[i].size.x >= window.getSize().x) {
                    velocities[i].speed.x = -velocities[i].speed.x;
                }
                if (transforms[i].position.y <= 0) {
                    velocities[i].speed.y = -velocities[i].speed.y;
                }

                // Ball collision with platform
                if (transforms[i].position.y + transforms[i].size.y >= transforms[1].position.y &&  // Assuming platform is second entity
                    transforms[i].position.x + transforms[i].size.x >= transforms[1].position.x &&
                    transforms[i].position.x <= transforms[1].position.x + transforms[1].size.x) {
                    transforms[i].position.y = transforms[1].position.y - transforms[i].size.y;
                    velocities[i].speed.y = -velocities[i].speed.y;
                }

                // Check collisions with bricks
                for (size_t j = 2; j < transforms.size(); ++j) {
                    if (behaviors[j].type == Behavior::Brick) {
                        if (checkCollision(transforms[i], transforms[j])) {
                            handleBallBrickCollision(transforms, velocities, renderables, behaviors, i, j);
                        }
                    }
                }

                // Reset ball if it goes below the window
                if (transforms[i].position.y > window.getSize().y) {
                    transforms[i].position = sf::Vector2f(957.f, 900.f);
                    velocities[i].speed = sf::Vector2f(400.f, -400.f);

                    // Reset the platform's position (assuming platform is the second entity)
                    transforms[1].position = sf::Vector2f(900.f, 950.f); // Platform initial position
                    velocities[1].speed = sf::Vector2f(600.f, 0); // Platform doesn't move vertically
                    platformMoved = false;
                }
            }
        }

        gameWon = true;
        for (size_t i = 2; i < transforms.size(); ++i) {
            if (behaviors[i].type == Behavior::Brick) {
                gameWon = false;  // There are still bricks left
                break;
            }
        }

        // Check if the Enter key is pressed
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            // Change background to victory texture
            Won = true;

            // Remove all bricks from ECS components
            for (size_t i = 2; i < behaviors.size(); ++i) {
                if (behaviors[i].type == Behavior::Brick) {
                    // Remove the brick from all ECS components
                    transforms.erase(transforms.begin() + i);
                    velocities.erase(velocities.begin() + i);
                    renderables.erase(renderables.begin() + i);
                    behaviors.erase(behaviors.begin() + i);

                    // Adjust index after removal since we've just erased an element
                    --i;
                }
            }
        }

        // Change background if game is won
        if (gameWon or Won) {
            sprite.setTexture(textureVictoire); // Change background to victory texture
            removePlayer(transforms, velocities, renderables, behaviors);
        }
        else {
            sprite.setTexture(texture); // Otherwise use the regular background
        }

        // Update the position of shapes
        for (size_t i = 0; i < renderables.size(); ++i) {
            // Get the drawable (either sf::Shape or sf::Sprite)
            sf::Drawable* drawable = renderables[i].getDrawable();
            if (drawable) {
                // Update the position based on the transform
                if (sf::Shape* shape = dynamic_cast<sf::Shape*>(drawable)) {
                    shape->setPosition(transforms[i].position);
                }
                else if (sf::Sprite* sprite = dynamic_cast<sf::Sprite*>(drawable)) {
                    sprite->setPosition(transforms[i].position);
                }
            }
        }

        // Rendu
        window.clear(sf::Color(224, 220, 220));  // Clear with a background color
        window.draw(sprite);

        // Draw all renderable objects from the ECS system
        for (size_t i = 0; i < renderables.size(); ++i) {
            sf::Drawable* drawable = renderables[i].getDrawable();
            if (drawable) {
                window.draw(*drawable);  // Draw the object
            }
        }

        window.display();
    }

    // Clean up dynamically allocated memory (for sprites and shapes)
    for (auto& renderable : renderables) {
        delete renderable.drawable;
    }

    return 0;
}
