#include "Box.h"

void box(int x1, int x2, int y1, int y2, sf::RenderWindow &window) {
  sf::Vertex line1[] = {sf::Vertex(sf::Vector2f(x1, y1)),
                        sf::Vertex(sf::Vector2f(x1, y2))};
  window.draw(line1, 2, sf::Lines);

  sf::Vertex line2[] = {sf::Vertex(sf::Vector2f(x1, y1)),
                        sf::Vertex(sf::Vector2f(x2, y1))};
  window.draw(line2, 2, sf::Lines);

  sf::Vertex line3[] = {sf::Vertex(sf::Vector2f(x2, y2)),
                        sf::Vertex(sf::Vector2f(x2, y1))};
  window.draw(line3, 2, sf::Lines);

  sf::Vertex line4[] = {sf::Vertex(sf::Vector2f(x1, y2)),
                        sf::Vertex(sf::Vector2f(x2, y2))};
  window.draw(line4, 2, sf::Lines);
}
