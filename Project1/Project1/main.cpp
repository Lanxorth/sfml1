#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>             // Include your brick header if needed

// Structures de base pour ECS
struct Transform2D {
    sf::Vector2f position;
    sf::Vector2f size;
};

struct Velocity {
    sf::Vector2f speed;
};

struct RenderComponent {
    sf::Drawable* drawable;  // This can hold either an sf::Shape* or an sf::Sprite*

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

int main() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Casse-Brique", sf::Style::Fullscreen);
    sf::Clock clock;

    // Load background texture
    sf::Texture texture;
    if (!texture.loadFromFile("background.jpg")) {
        return 0;
    }

    // Load brick texture
    sf::Texture textureBrick;
    if (!textureBrick.loadFromFile("Brick2.png")) {
        std::cerr << "Error loading texture!" << std::endl;
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

    // Create a grid of bricks with fixed size
    createEntitiesBricksGrid(transforms, velocities, renderables, behaviors, 100, 100, 10, 16, 10, brickWidth, brickHeight, textureBrick);

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        // Event handling
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                window.close();
            }
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
