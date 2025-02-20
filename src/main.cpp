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
  float ballRadius = 25.f;
  int numParticles = 1000;
  float dampingRatio = 1.0f;
  float gravity = 0.f;
  float spacing = 5.0f;
  float smoothingRadius = 50.0f;

  // Liste de balles
  std::vector<Balle> balles;

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
    for (auto &balle : balles) {

      sf::Vector2f vel = balle.getVelocity();
      vel.y += gravity * delta;
      balle.setVelocity(vel);
      balle.update(delta);
      box.checkCollision(balle, dampingRatio);

    }

    Balle balleDensité = balles[510];
    sf::Vector2f position = balleDensité.getPosition();

    float densité = balleDensité.calculateDensity(balles, smoothingRadius);

      ImGui::SFML::Update(window, deltaClock.restart());

    ImGui::Begin("Simulation Settings");
    ImGui::SliderFloat("SmoothingRadius", &smoothingRadius, 25.0f, 500.0f);
    ImGui::SliderFloat("Spacing", &spacing, 5.0f, 50.0f);
    ImGui::Text("Density: %.6f", densité);
    ImGui::End();

    window.clear();
    for (auto &balle : balles) {
      balle.draw(window);
    }
    // box(x1, x2, y1, y2, window);
    box.draw(window);

    // Draw the circle at balleDensité's position with smoothingRadius
    sf::CircleShape densityCircle(smoothingRadius);
    densityCircle.setOrigin(smoothingRadius, smoothingRadius);
    densityCircle.setPosition(position);
    densityCircle.setFillColor(sf::Color(0, 0, 255, 100)); // Semi-transparent blue
    window.draw(densityCircle);

      ImGui::SFML::Render(window);
    window.display();
  }
    ImGui::SFML::Shutdown();
  return 0;
}
