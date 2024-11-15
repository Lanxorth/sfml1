#include "Systems.h"

// Système de rendu
void renderSystem(sf::RenderWindow& window, const std::vector<RenderComponent>& renderables) {
    for (const auto& renderable : renderables) {
        if (renderable.drawable) {
            window.draw(*renderable.drawable);
        }
    }
}

// Système de mouvement
void movementSystem(std::vector<Transform2D>& transforms, const std::vector<Velocity>& velocities, float deltaTime) {
    for (size_t i = 0; i < transforms.size(); ++i) {
        transforms[i].position += velocities[i].speed * deltaTime;
    }
}

// Système de collision (Ball <-> Brick)
bool checkCollision(const Transform2D& ball, const Transform2D& brick) {
    return ball.position.x < brick.position.x + brick.size.x &&
        ball.position.x + ball.size.x > brick.position.x &&
        ball.position.y < brick.position.y + brick.size.y &&
        ball.position.y + ball.size.y > brick.position.y;
}

void handleBallBrickCollision(
    std::vector<Transform2D>& transforms,
    std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables,
    std::vector<Behavior>& behaviors,
    size_t ballIndex,
    size_t brickIndex
) {
    Transform2D& ballTransform = transforms[ballIndex];
    Velocity& ballVelocity = velocities[ballIndex];
    Transform2D& brickTransform = transforms[brickIndex];

    // Détection de la direction de collision
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
            ballVelocity.speed.x = -ballVelocity.speed.x; // Rebond sur les côtés
        }
        else {
            ballVelocity.speed.y = -ballVelocity.speed.y; // Rebond en haut ou en bas
        }
    }

    // Supprime la brique après collision
    transforms.erase(transforms.begin() + brickIndex);
    velocities.erase(velocities.begin() + brickIndex);
    renderables.erase(renderables.begin() + brickIndex);
    behaviors.erase(behaviors.begin() + brickIndex);
}

// Système de collision avec les murs
void handleBallWallCollision(Transform2D& ballTransform, Velocity& ballVelocity, const sf::RenderWindow& window) {
    if (ballTransform.position.x <= 0 || ballTransform.position.x + ballTransform.size.x >= window.getSize().x) {
        ballVelocity.speed.x = -ballVelocity.speed.x; // Rebond sur les bords gauche/droit
    }
    if (ballTransform.position.y <= 0) {
        ballVelocity.speed.y = -ballVelocity.speed.y; // Rebond sur le haut
    }
}

// Système de collision (Ball <-> Platform)
void handleBallPlatformCollision(Transform2D& ballTransform, Velocity& ballVelocity, const Transform2D& platformTransform) {
    if (ballTransform.position.y + ballTransform.size.y >= platformTransform.position.y &&
        ballTransform.position.x + ballTransform.size.x >= platformTransform.position.x &&
        ballTransform.position.x <= platformTransform.position.x + platformTransform.size.x) {

        ballTransform.position.y = platformTransform.position.y - ballTransform.size.y; // Ajuster la position de la balle
        ballVelocity.speed.y = -ballVelocity.speed.y; // Inverser la direction verticale
    }
}

// Réinitialiser la balle et la plateforme
void resetBallAndPlatform(
    Transform2D& ballTransform,
    Velocity& ballVelocity,
    Transform2D& platformTransform,
    Velocity& platformVelocity
) {
    ballTransform.position = { 957.f, 900.f };
    ballVelocity.speed = { 400.f, -400.f };

    platformTransform.position = { 900.f, 950.f };
    platformVelocity.speed = { 600.f, 0.f };
}