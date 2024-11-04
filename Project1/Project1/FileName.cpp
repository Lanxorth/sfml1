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
    sf::Shape* shape;
};

struct Behavior {
    enum Type { Ball, Paddle, Brick } type;
};

struct Render {
    enum Shape { Circle, Rectangle };
    Shape shape; // Shape entity
    sf::Color color;
    sf::CircleShape circleShape;
    sf::RectangleShape rectangleShape;

    // Constructor for Circle and Rectangle
    Render(float radius, const sf::Color& color)
        : shape(Circle), color(color), circleShape(radius) {
        circleShape.setFillColor(color);
    }

    Render(const sf::Vector2f& size, const sf::Color& color)
        : shape(Rectangle), color(color), rectangleShape(size) {
        rectangleShape.setFillColor(color);
    }
};

// Fonctions des systèmes
void renderSystem1(sf::RenderWindow& window, const std::vector<RenderComponent>& renderables) {
    for (const auto& renderable : renderables) {
        window.draw(*renderable.shape);
    }
}

void movementSystem(std::vector<Transform2D>& transforms, const std::vector<Velocity>& velocities, float deltaTime) {
    for (size_t i = 0; i < transforms.size(); ++i) {
        transforms[i].position += velocities[i].speed * deltaTime;
    }
}

void collisionSystem(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities, float windowHeight, float windowWidth) {
    for (size_t i = 0; i < transforms.size(); ++i) {
        // Vérification des collisions avec les murs
        if (transforms[i].position.x < 0 || transforms[i].position.x + transforms[i].size.x > windowWidth) {
            velocities[i].speed.x *= -1;
        }
        if (transforms[i].position.y < 0 || transforms[i].position.y + transforms[i].size.y > windowHeight) {
            velocities[i].speed.y *= -1;
        }
    }
}

void handleInput(Transform2D& paddleTransform, float paddleSpeed, float deltaTime) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        paddleTransform.position.x -= paddleSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        paddleTransform.position.x += paddleSpeed * deltaTime;
    }
}

// Création des entités et ajout de leurs composants
/*void createEntities(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors) {

    // Création de la balle
    Transform2D ballTransform{ sf::Vector2f(400, 300), sf::Vector2f(10, 10) };
    Velocity ballVelocity{ sf::Vector2f(300, 100) };
    sf::CircleShape* ballShape = new sf::CircleShape(10);
    ballShape->setFillColor(sf::Color::White);
    RenderComponent ballRender{ ballShape };
    Behavior ballBehavior{ Behavior::Ball };

    transforms.push_back(ballTransform);
    velocities.push_back(ballVelocity);
    renderables.push_back(ballRender);
    behaviors.push_back(ballBehavior);

    // Création de la raquette
    Transform2D paddleTransform{ sf::Vector2f(350, 550), sf::Vector2f(100, 20) };
    sf::RectangleShape* paddleShape = new sf::RectangleShape(sf::Vector2f(100, 20));
    paddleShape->setFillColor(sf::Color::Blue);
    RenderComponent paddleRender{ paddleShape };
    Behavior paddleBehavior{ Behavior::Paddle };

    transforms.push_back(paddleTransform);
    velocities.emplace_back(); // Pas de vitesse pour la raquette
    renderables.push_back(paddleRender);
    behaviors.push_back(paddleBehavior);
}*/

void createEntityBrick(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors,
    float x, float y, float brickWidth, float brickHeight) {

    // Create a brick
    Transform2D brickTransform{ sf::Vector2f(x, y), sf::Vector2f(brickWidth, brickHeight) };
    Velocity brickVelocity{ sf::Vector2f(0, 0) };
    sf::RectangleShape* brickShape = new sf::RectangleShape(sf::Vector2f(brickWidth, brickHeight));
    brickShape->setFillColor(sf::Color(119, 76, 194));
    RenderComponent brickRender{ brickShape };
    Behavior brickBehavior{ Behavior::Brick };

    transforms.push_back(brickTransform);
    velocities.push_back(brickVelocity);
    renderables.push_back(brickRender);
    behaviors.push_back(brickBehavior);
}

void createEntitiesBricksGrid(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors,
    float startX, float startY, int rows, int cols, float spacing, float brickWidth, float brickHeight) {

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            float x = startX + col * (100.0f + spacing); // Fixed width
            float y = startY + row * (20.0f + spacing);  // Fixed height
            createEntityBrick(transforms, velocities, renderables, behaviors, x, y, brickWidth, brickHeight);
        }
    }
}

int main() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Casse-Brique", sf::Style::Fullscreen);
    sf::Clock clock;

    sf::Texture texture;
    if (!texture.loadFromFile("background.jpg"))
    {
        return 0; 
    }

    sf::Sprite sprite;
    sprite.setTexture(texture);

    // Composants ECS
    std::vector<Transform2D> transforms;
    std::vector<Velocity> velocities;
    std::vector<RenderComponent> renderables;
    std::vector<Behavior> behaviors;

    // Create entities for Circle and Rectangle
    Render circleRender(20.f, sf::Color::White);
    Render rectangleRender(sf::Vector2f(150.f, 30.f), sf::Color::Cyan);

    // Set the initial position of the circle and rectangle
    circleRender.circleShape.setPosition(950.f, 900.f);
    rectangleRender.rectangleShape.setPosition(900.f, 950.f);

    // Draw the shapes from the Render components
    window.draw(circleRender.circleShape);
    window.draw(rectangleRender.rectangleShape);

    float brickWidth = window.getSize().y * 0.095f;  // 10% of window width
    float brickHeight = window.getSize().x * 0.014f; // 5% of window height

    // Create a grid of bricks with fixed size
    createEntitiesBricksGrid(transforms, velocities, renderables, behaviors,100, 100, 10, 16, 10, brickWidth, brickHeight);

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        // Event handling
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        window.close();
        }

        // Handle paddle input
        handleInput(transforms[1], 200.0f, deltaTime); // Paddle is at index 1

        // ECS systems
        movementSystem(transforms, velocities, deltaTime);
        collisionSystem(transforms, velocities, window.getSize().y, window.getSize().x);

        // Update the position of shapes
        for (size_t i = 0; i < renderables.size(); ++i) {
            renderables[i].shape->setPosition(transforms[i].position);
        }

        // Rendu
        window.clear(sf::Color(224, 220, 220));
        window.draw(sprite);
        renderSystem1(window, renderables);
        window.display();
    }

    

    // Nettoyage de la mémoire allouée pour les formes
    for (auto& renderable : renderables) {
        delete renderable.shape;
    }

    return 0;
}

/*int main()
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Scaled Window", sf::Style::Fullscreen);
    sf::VertexArray triangleStrip(sf::TriangleStrip, 4);
    triangleStrip[0].position = sf::Vector2f(10.f, 10.f);
    triangleStrip[1].position = sf::Vector2f(10.f, 110.f);
    triangleStrip[2].position = sf::Vector2f(210.f, 10.f);
    triangleStrip[3].position = sf::Vector2f(210.f, 110.f);
    triangleStrip[0].texCoords = sf::Vector2f(0.f, 0.f);
    triangleStrip[1].texCoords = sf::Vector2f(0.f, 50.f);
    triangleStrip[2].texCoords = sf::Vector2f(25.f, 0.f);
    triangleStrip[3].texCoords = sf::Vector2f(25.f, 50.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();

        }

        // Clear the window
        window.clear(sf::Color::Black);

        // Draw the circle
        window.draw(triangleStrip);

        // Display the contents of the window
        window.display();
    }


    return 0;
}*/