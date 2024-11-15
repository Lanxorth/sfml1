#pragma once
#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Components.h"

void renderSystem(sf::RenderWindow& window, const std::vector<RenderComponent>& renderables);
bool checkCollision(const Transform2D& ball, const Transform2D& brick);
void handleBallBrickCollision(std::vector<Transform2D>& transforms, std::vector<Velocity>& velocities,
    std::vector<RenderComponent>& renderables, std::vector<Behavior>& behaviors, size_t ballIndex, size_t brickIndex);

#endif // SYSTEMS_H
