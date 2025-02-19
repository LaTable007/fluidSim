#include "Balle.h"
#include "Box.h"
#include "Start.h"
#include "StartRandom.h"
#include <SFML/Graphics.hpp>

int main() {
  sf::VideoMode fullscreenMode = sf::VideoMode::getDesktopMode();
  sf::RenderWindow window(fullscreenMode, "Simulation de Fluide", sf::Style::Fullscreen);

  float x1 = 50;
  float x2 = 1390;
  float y1 = 50;
  float y2 = 850;

  Box box(x1, x2, y1, y2);

  // Paramètres pour les balles
  float ballRadius = 25.f;
  int numParticles = 500;
  float dampingRatio = 0.7f;
  float gravity = 10.f;

  // Liste de balles
  std::vector<Balle> balles;

  // Initialiser les balles
  // Start(balles, numParticles, ballRadius, spacing);
  startRandom(balles, numParticles, ballRadius, box);

  sf::Clock clock;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    float delta = clock.restart().asSeconds();

    // Mettre à jour toutes les balles
    for (auto &balle : balles) {

      sf::Vector2f vel = balle.getVelocity();
      vel.y += gravity * delta;
      balle.setVelocity(vel);
      balle.update(delta);
      box.checkCollision(balle, dampingRatio);

    }

    // Rendu
    window.clear();
    for (auto &balle : balles) {
      balle.draw(window);
    }
    // box(x1, x2, y1, y2, window);
    box.draw(window);
    window.display();
  }
  return 0;
}
