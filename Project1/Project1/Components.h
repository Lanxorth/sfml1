#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <SFML/Graphics.hpp>

struct Transform2D {
    sf::Vector2f position;
    sf::Vector2f size;
};

struct Velocity {
    sf::Vector2f speed;
};

struct RenderComponent {
    sf::Drawable* drawable;

    RenderComponent(sf::Shape* shape) : drawable(shape) {}
    RenderComponent(sf::Sprite* sprite) : drawable(sprite) {}

    sf::Drawable* getDrawable() const {
        return drawable;
    }
};

struct Behavior {
    enum Type { Ball, Platform, Brick } type;
};

#endif // COMPONENTS_H
