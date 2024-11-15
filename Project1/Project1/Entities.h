#ifndef ENTITIES_H
#define ENTITIES_H

#include <vector>
#include "Components.h"

// Fonctions pour la gestion des entités
void createEntityBall(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors, const sf::Texture& textureBall);

void createEntityPlatform(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors, const sf::Texture& texturePlatform);

void createEntityBrick(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors,
    float x, float y, float brickWidth, float brickHeight, const sf::Texture& textureBrick);

void createEntitiesBricksGrid(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors,
    float startX, float startY, int rows, int cols, float spacing, float brickWidth, float brickHeight, const sf::Texture& textureBrick);

void removeEntity(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors, size_t index);

void removePlayer(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors);

#endif // ENTITIES_H
