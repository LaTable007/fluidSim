#ifndef BOX_HPP
#define BOX_HPP

#include <SFML/Graphics.hpp>
#include "Balle.h"

class Box {
private:
    float x1; float x2; float y1; float y2;
    sf::RectangleShape shape;
public:
    Box(float x1, float x2, float y1, float y2);
    void draw(sf::RenderWindow &window);
    void checkCollision(Balle &ball, float dampingRatio);
    void getBounds(float* outX1, float* outX2, float* outY1, float* outY2) const;
    void checkCollisionPredPosition(Balle &balle);

};

#endif
