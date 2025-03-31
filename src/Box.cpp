#include "../include/Box.h"
#include "../include/Balle.h"

Box::Box(float x1, float x2, float y1, float y2)
    : x1(x1), x2(x2), y1(y1), y2(y2) {
  // Calculer le coin en haut à gauche et la taille du rectangle
  float left = std::min(x1, x2);
  float top = std::min(y1, y2);
  float width = std::abs(x2 - x1);
  float height = std::abs(y2 - y1);

  // Configurer l'objet RectangleShape
  shape.setPosition(left, top);
  shape.setSize(sf::Vector2f(width, height));
  shape.setFillColor(sf::Color::Transparent);
  shape.setOutlineThickness(2.f);
  shape.setOutlineColor(sf::Color::White);
}

void Box::draw(sf::RenderWindow &window) { window.draw(shape); }

void Box::checkCollision(Balle &balle, float dampingRatio) {
  // Récupérer la position et le rayon de la balle
  sf::Vector2f pos = balle.getPosition();
  sf::Vector2f vel = balle.getVelocity();
  float ballRadius = balle.getRadius();

  if (pos.x - ballRadius <= x1) {
    vel.x = -vel.x * dampingRatio;
    vel.y = vel.y * dampingRatio;
    pos.x = x1 + ballRadius;
  }
  if (pos.x + ballRadius >= x2) {
    vel.x = -vel.x * dampingRatio;
    vel.y = vel.y * dampingRatio;
    pos.x = x2 - ballRadius;
  }
  if (pos.y - ballRadius <= y1) {
    vel.x = vel.x * dampingRatio;
    vel.y = -vel.y * dampingRatio;
    pos.y = y1 + ballRadius;
  }
  if (pos.y + ballRadius >= y2) {
    vel.x = vel.x * dampingRatio;
    vel.y = -vel.y * dampingRatio;
    pos.y = y2 - ballRadius;
  }
  balle.setVelocity(vel);
  balle.setPosition(pos);
}

void Box::checkCollisionPredPosition(Balle &balle) {
  // Récupérer la position et le rayon de la balle
  sf::Vector2f pos = balle.getPredPosition();
  float ballRadius = balle.getRadius();

  if (pos.x - ballRadius <= x1) {
    pos.x = x1 + ballRadius;
  }
  if (pos.x + ballRadius >= x2) {
    pos.x = x2 - ballRadius;
  }
  if (pos.y - ballRadius <= y1) {
    pos.y = y1 + ballRadius;
  }
  if (pos.y + ballRadius >= y2) {
    pos.y = y2 - ballRadius;
  }
  balle.setPredPosition(pos);
}

void Box::getBounds(float *outX1, float *outX2, float *outY1,
                    float *outY2) const {
  if (outX1)
    *outX1 = x1;
  if (outX2)
    *outX2 = x2;
  if (outY1)
    *outY1 = y1;
  if (outY2)
    *outY2 = y2;
}
