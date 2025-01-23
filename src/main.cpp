#include "Balle.h"
#include "Box.h"
#include "Start.h"
#include "StartRandom.h"
#include <SFML/Graphics.hpp>

int main() {
  sf::RenderWindow window(sf::VideoMode(800, 600), "Simulation de Fluide");

  int x1 = 000;
  int x2 = 800;
  int y1 = 000;
  int y2 = 600;

  // Paramètres pour les balles
  float ballRadius = 10.f;
  int numParticles = 100;
  float spacing = 5.f;
  double dampingRatio = 1.0;
  double gravity = 0.f;

  // Liste de balles
  std::vector<Balle> balles;

  // Initialiser les balles
  // Start(balles, numParticles, ballRadius, spacing);
  startRandom(balles, numParticles, ballRadius);

  sf::Clock clock;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    double delta = clock.restart().asSeconds();

    // Mettre à jour toutes les balles
    for (auto &balle : balles) {
      balle.update(delta);

      sf::Vector2f pos = balle.getPosition();
      sf::Vector2f vel = balle.getVelocity();

      // Vérifier les collisions avec les bords de la boîte
      if (pos.x <= x1 || pos.x + 2 * ballRadius >= x2) {
        vel.x = -vel.x * dampingRatio;
      }
      if (pos.y <= y1 || pos.y + 2 * ballRadius >= y2) {
        vel.y = -vel.y * dampingRatio;
      }
      vel.y += gravity * delta;

      balle.setVelocity(vel);
    }

    // Rendu
    window.clear();
    for (auto &balle : balles) {
      balle.draw(window);
    }
    // box(x1, x2, y1, y2, window);
    window.display();
  }
  return 0;
}
