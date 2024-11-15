#include "Entities.h"
#include <SFML/Graphics.hpp>

// Gestion de la création des entités
void createEntityBall(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors, const sf::Texture& textureBall) {

    Transform2D ballTransform{ sf::Vector2f(957.f, 900.f), sf::Vector2f(40.f, 40.f) };
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

void createEntityBrick(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors,
    float x, float y, float brickWidth, float brickHeight, const sf::Texture& textureBrick) {

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
            float x = startX + col * (brickWidth + spacing);
            float y = startY + row * (brickHeight + spacing);
            createEntityBrick(transforms, velocities, renderables, behaviors, x, y, brickWidth, brickHeight, textureBrick);
        }
    }
}

// Suppression des entités
void removeEntity(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors, size_t index) {

    transforms.erase(transforms.begin() + index);
    velocities.erase(velocities.begin() + index);
    delete renderables[index].drawable; // Libère la mémoire du drawable
    renderables.erase(renderables.begin() + index);
    behaviors.erase(behaviors.begin() + index);
}

void removePlayer(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors) {

    for (size_t i = 0; i < behaviors.size(); ++i) {
        if (behaviors[i].type == Behavior::Ball || behaviors[i].type == Behavior::Platform) {
            removeEntity(transforms, velocities, renderables, behaviors, i);
            --i; // Ajuste l'indice après la suppression
        }
    }
}
