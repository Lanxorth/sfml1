#include <SFML/Graphics.hpp>
#include "Components.h"
#include "Systems.h"
#include "Entities.h"

int main() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Casse-Brique", sf::Style::Fullscreen);
    sf::Clock clock;

    // ECS Components
    std::vector<Transform2D> transforms;
    std::vector<Velocity> velocities;
    std::vector<RenderComponent> renderables;
    std::vector<Behavior> behaviors;

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

    // Define brick dimensions based on window size
    float brickWidth = window.getSize().y * 0.095f;
    float brickHeight = window.getSize().x * 0.014f;

    createEntityBall(transforms, velocities, renderables, behaviors, textureBall);
    createEntityPlatform(transforms, velocities, renderables, behaviors, texturePlatform);
    bool platformMoved = false;


    // Create a grid of bricks with fixed size
    createEntitiesBricksGrid(transforms, velocities, renderables, behaviors, 100, 100, 10, 15, 14, brickWidth, brickHeight, textureBrick);
    bool gameWon = true;

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

        for (size_t i = 2; i < transforms.size(); ++i) {
            if (behaviors[i].type == Behavior::Brick) {
                gameWon = false;  // There are still bricks left
                break;
            }
        }

        // Check if the Enter key is pressed
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            // Change background to victory texture
            gameWon = true;

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
        if (gameWon) {
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
