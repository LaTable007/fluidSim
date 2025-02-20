#include "../include/Balle.h"
#include "../include/Box.h"
#include "../include/Start.h"
#include "../include/StartRandom.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <imgui-SFML.h>
#include <imgui.h>

int main() {
  sf::VideoMode fullscreenMode = sf::VideoMode::getDesktopMode();
  sf::RenderWindow window(fullscreenMode, "ImGui + SFML = <3", sf::Style::Fullscreen);
  window.setFramerateLimit(60);
  ImGui::SFML::Init(window);

  ImGui::GetIO().FontGlobalScale = 2.0f; // Ajustez ce facteur selon vos besoins

  float x1 = 50;
  float x2 = 2825;
  float y1 = 50;
  float y2 = 1750;

  Box box(x1, x2, y1, y2);

  // Paramètres pour les balles
  float ballRadius = 5.f;
  int numParticles = 100;
  float dampingRatio = 1.0f;
  float gravity = 0.f;
  float spacing = 10.0f;
  float smoothingRadius = 50.0f;
  float targetDensity = 1.0f;
  float pressureMultiplier = 1.0f;
  float mass = 1.0f;

  // Liste de balles
  std::vector<Balle> balles;
  std::vector<sf::Vector2f> pressureForces(numParticles, sf::Vector2f(0.f, 0.f));

  // Initialiser les balles
  // Start(balles, numParticles, ballRadius, spacing);
  // startRandom(balles, numParticles, ballRadius, box);


  sf::Clock deltaClock;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);

      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
    float delta = deltaClock.restart().asSeconds();

    Start(balles, numParticles, ballRadius, spacing, box);

    // Mettre à jour toutes les balles
    for (int index = 0; index <= numParticles; index++) {
      pressureForces[index] = balles[index].calculatePressureForce(balles, index, numParticles, smoothingRadius, mass, targetDensity, pressureMultiplier);
    }



    ImGui::SFML::Update(window, deltaClock.restart());

    ImGui::Begin("Simulation Settings");
    ImGui::End();

    window.clear();
    for (auto &balle : balles) {
      balle.draw(window);
    }
    // box(x1, x2, y1, y2, window);

    sf::VertexArray lines(sf::Lines);

    for (size_t i = 0; i < balles.size(); ++i) {
      sf::Vector2f start = balles[i].getPosition();
      sf::Vector2f end = balles[i].getPosition() + pressureForces[i];

      // Définir les deux sommets de la ligne
      lines.append(sf::Vertex(start, sf::Color::Red));
      lines.append(sf::Vertex(end, sf::Color::Blue));
    }

    window.draw(lines);
    box.draw(window);


      ImGui::SFML::Render(window);
    window.display();
  }
    ImGui::SFML::Shutdown();
  return 0;
}
