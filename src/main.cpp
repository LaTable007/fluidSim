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
    ImGui::GetIO().FontGlobalScale = 2.0f;

    float x1 = 100, x2 = 2825, y1 = 50, y2 = 1750;
    Box box(x1, x2, y1, y2);

    // Paramètres pour les balles
    float ballRadius = 50.f;
    int numParticles = 100;
    float dampingRatio = 0.8f;
    float spacing = 5.0f;
    float smoothingRadius = 500.0f;
    float targetDensity = 1.0f;
    float pressureMultiplier = 10000.0f;
    float mass = 0.001f;
    sf::Vector2f gravity = sf::Vector2f(0.f, 9.8f);

    // Variables de contrôle ImGui
    bool showCircle = false;
    int selectedParticle = 0;
    bool paused = false; // Variable pour gérer la pause de la simulation

    // Liste de balles
    std::vector<Balle> balles;
    std::vector<sf::Vector2f> pressureForces(numParticles, sf::Vector2f(0.f, 0.f));
    Start(balles, numParticles, ballRadius, spacing, box);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Récupérer le temps écoulé
        sf::Time delta = deltaClock.restart();
        float dt = delta.asSeconds();
        float fps = 1.0f / dt;

        // Mise à jour de la simulation seulement si elle n'est pas en pause
        if (!paused) {
            for (int index = 0; index < numParticles; index++) {
                pressureForces[index] = balles[index].calculatePressureForce(
                    balles, index, numParticles, smoothingRadius, mass, targetDensity, pressureMultiplier);
                sf::Vector2f pressureAcceleration = pressureForces[index] / mass;
                sf::Vector2f vel = balles[index].getVelocity() + pressureAcceleration * dt;
                //vel += gravity * dt;
                balles[index].setVelocity(vel);
                balles[index].update(dt);
                box.checkCollision(balles[index], dampingRatio);
            }
        }

        // Mise à jour d'ImGui
        ImGui::SFML::Update(window, delta);

        // Interface ImGui
        ImGui::Begin("Simulation Settings");

        // Bouton pause / reprise
        if (ImGui::Button(paused ? "Resume Simulation" : "Pause Simulation"))
            paused = !paused;

        ImGui::Checkbox("Afficher cercle", &showCircle);
        if (showCircle)
            ImGui::SliderInt("Particule sélectionnée", &selectedParticle, 0, numParticles - 1);
        if (showCircle && selectedParticle >= 0 && selectedParticle < balles.size()) {
            sf::Vector2f pos = balles[selectedParticle].getPosition();
            sf::Vector2f vel = balles[selectedParticle].getVelocity();
            ImGui::Text("Position: (%.2f, %.2f)", pos.x, pos.y);
            ImGui::Text("Vitesse: (%.2f, %.2f)", vel.x, vel.y);
        }
        ImGui::SliderFloat("PressureMultiplier", &pressureMultiplier, 1.0f, 1000000.0f);
        ImGui::SliderFloat("SmoothingRadius", &smoothingRadius, 50.0f, 5000.0f);
        ImGui::SliderFloat("TargetDensity", &targetDensity, 0.1f, 10.0f);
        ImGui::Text("FPS: %.1f", fps);
        if (paused)
            ImGui::Text("Simulation en pause");


        ImGui::End();

        window.clear();

        // Dessiner les balles
        for (auto &balle : balles)
            balle.draw(window);

        // Dessiner les forces de pression
        sf::VertexArray lines(sf::Lines);
        for (size_t i = 0; i < balles.size(); ++i) {
            sf::Vector2f start = balles[i].getPosition();
            sf::Vector2f end = balles[i].getPosition() + pressureForces[i];
            lines.append(sf::Vertex(start, sf::Color::Red));
            lines.append(sf::Vertex(end, sf::Color::Blue));
        }
        window.draw(lines);
        box.draw(window);

        // Si "Afficher cercle" est coché, tracer le cercle et le vecteur de vitesse
        if (showCircle && selectedParticle >= 0 && selectedParticle < balles.size()) {
            // Dessiner le cercle autour de la balle sélectionnée
            sf::CircleShape circle(smoothingRadius);
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineColor(sf::Color::Green);
            circle.setOutlineThickness(2);
            circle.setOrigin(smoothingRadius, smoothingRadius);
            circle.setPosition(balles[selectedParticle].getPosition());
            window.draw(circle);

            // Tracer le vecteur de vitesse (ligne jaune)
            sf::Vector2f pos = balles[selectedParticle].getPosition();
            sf::Vector2f vel = balles[selectedParticle].getVelocity();
            sf::VertexArray velocityLine(sf::Lines, 2);
            velocityLine[0].position = pos;
            velocityLine[0].color = sf::Color::Yellow;
            velocityLine[1].position = pos + vel;
            velocityLine[1].color = sf::Color::Yellow;
            window.draw(velocityLine);
        }

        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}
