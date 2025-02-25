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
    ImGui::SFML::Init(window);
    ImGui::GetIO().FontGlobalScale = 2.0f;

    float x1 = 100, x2 = 2825, y1 = 50, y2 = 1750;
    Box box(x1, x2, y1, y2);

    // Paramètres pour les balles
    float ballRadius = 50.f;
    int numParticles = 500;
    float dampingRatio = 1.0f;
    float spacing = 5.0f;
    float smoothingRadius = 250.0f;
    float targetDensity = 1.0f;
    float pressureMultiplier = 0.1f;
    float mass = 1.0f;
    sf::Vector2f gravity = sf::Vector2f(0.f, 0.0f);

    // Variables de contrôle ImGui
    bool showCircle = false;
    int selectedParticle = 0;
    bool paused = false; // Variable pour gérer la pause de la simulation

    // Liste de balles
    std::vector<Balle> balles;
    std::vector<sf::Vector2f> pressureForces(numParticles, sf::Vector2f(0.f, 0.f));
    startRandom(balles, numParticles, ballRadius, box);

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
            // Mise à jour des densités pour toutes les balles
            for (int index = 0; index < numParticles; index++) {
                sf::Vector2f vel =  balles[index].getVelocity() + gravity * dt;
                sf::Vector2f predictedPos = balles[index].getPosition() + vel * 1.0f / 120.f;
                balles[index].setPredPosition(predictedPos);
                balles[index].updateDensity(balles, smoothingRadius, index, mass);
                balles[index].setVelocity(vel);
            }

            for (int index = 0; index < numParticles; index++) {
                pressureForces[index] = balles[index].calculatePressureForce(
                    balles, index, numParticles, smoothingRadius, mass, targetDensity, pressureMultiplier);
                sf::Vector2f pressureAcceleration = pressureForces[index] / balles[index].getDensity();
                sf::Vector2f vel  =  balles[index].getVelocity() + pressureAcceleration * dt;
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
            ImGui::Text("Densité: %.8f", balles[selectedParticle].getDensity());
        }
        ImGui::SliderFloat("PressureMultiplier", &pressureMultiplier, 0.1f, 20.0f);
        ImGui::SliderFloat("SmoothingRadius", &smoothingRadius, 50.0f, 500.0f);
        ImGui::SliderFloat("TargetDensity", &targetDensity, 0.1f, 100.0f);
        ImGui::SliderFloat("Gravity", &gravity.y, 0.0f, 100.0f);
        ImGui::SliderFloat("Mass", &mass, 0.01f, 10.0f);
        ImGui::Text("FPS: %.1f", fps);
        if (paused)
            ImGui::Text("Simulation en pause");


        ImGui::End();

        window.clear();

        // Dessiner les balles
        for (auto &balle : balles)
            balle.draw(window);

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