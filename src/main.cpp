#include "../include/Balle.h"
#include "../include/Box.h"
#include "../include/Start.h"
#include "../include/StartRandom.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>

int main() {
    sf::VideoMode fullscreenMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(fullscreenMode, "ImGui + SFML = <3", sf::Style::Fullscreen);
    ImGui::SFML::Init(window);
    ImGui::GetIO().FontGlobalScale = 2.0f;

    // Charger une police pour afficher les indices
    sf::Font font;

    // Spécifiez le chemin vers un fichier de police valide sur votre système
    bool fontLoaded = font.loadFromFile("../fonts/arial.ttf"); // Modifiez ce chemin selon votre configuration
    if (!fontLoaded) {
        std::cerr << "Erreur: Impossible de charger la police" << std::endl;
        // Fallback: Essayez de charger la police par défaut du système (chemin alternatif)
        fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf"); // Windows
        if (!fontLoaded) {
            fontLoaded = font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"); // Linux
            if (!fontLoaded) {
                fontLoaded = font.loadFromFile("/System/Library/Fonts/Helvetica.ttc"); // macOS
                if (!fontLoaded) {
                    std::cerr << "Aucune police de secours n'a pu être chargée. Les indices ne seront pas affichés." << std::endl;
                }
            }
        }
    }

    float x1 = 100, x2 = 2825, y1 = 50, y2 = 1750;
    Box box(x1, x2, y1, y2);

    // Paramètres pour les balles
    float ballRadius = 50.f;
    int numParticles = 10;
    float dampingRatio = 0.8f;
    float spacing = 5.0f;
    float smoothingRadius = 75.0f;
    float targetDensity = 1.0f;
    float pressureMultiplier = 1.0f;
    float mass = 0.01f;
    float mouseRadius = 200.0f;
    sf::Vector2f gravity = sf::Vector2f(0.f, 500.0f);

    std::unordered_map<unsigned int, int> spatialLookup;
    std::vector<unsigned int> startIndices(numParticles);

    // Variables de contrôle ImGui
    bool showCircle = false;
    int selectedParticle = 0;

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

        updateSpatialLookup(balles, smoothingRadius, numParticles, spatialLookup, startIndices);

     // Mise à jour d'ImGui
    ImGui::SFML::Update(window, delta);

    // Interface ImGui
    ImGui::Begin("Simulation Settings");


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
    ImGui::SliderFloat("SmoothingRadius", &smoothingRadius, 50.0f, 500.0f);

        // À ajouter dans votre section ImGui, avant ImGui::End();

        if (ImGui::CollapsingHeader("Spatial Lookup Data")) {
            static int numElements = 10;
            ImGui::SliderInt("Elements to show", &numElements, 1, 30);

            // Afficher spatialLookup
            ImGui::Text("spatialLookup (Key, Value):");
            int count = 0;
            for (const auto &pair : spatialLookup) {
                if (count >= numElements) break;
                ImGui::Text("(%d, %u)", pair.first, pair.second);

                if (count % 5 < 4) // Affiche plusieurs éléments par ligne
                    ImGui::SameLine();

                count++;
            }

            ImGui::Separator();

            // Afficher startIndices
            ImGui::Text("startIndices (Key, Value):");
            for (int i = 0; i < std::min(numElements, (int)startIndices.size()); i++) {
                if (startIndices[i] == INT_MAX)
                    ImGui::Text("(%d, MAX)", i);
                else
                    ImGui::Text("(%d, %u)", i, startIndices[i]);

                if (i % 5 < 4) // Affiche plusieurs éléments par ligne
                    ImGui::SameLine();
            }
        }



    ImGui::Text("FPS: %.1f", fps);

    ImGui::End();

    window.clear();


        // Dessiner le quadrillage
        int gridSizeX = window.getSize().x / smoothingRadius;
        int gridSizeY = window.getSize().y / smoothingRadius;

        // Dessiner les lignes verticales
        for (int i = 0; i <= gridSizeX; ++i) {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(i * smoothingRadius, 0), sf::Color::White),
                sf::Vertex(sf::Vector2f(i * smoothingRadius, window.getSize().y), sf::Color::White)
            };
            window.draw(line, 2, sf::Lines);
        }

        // Dessiner les lignes horizontales
        for (int i = 0; i <= gridSizeY; ++i) {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(0, i * smoothingRadius), sf::Color::White),
                sf::Vertex(sf::Vector2f(window.getSize().x, i * smoothingRadius), sf::Color::White)
            };
            window.draw(line, 2, sf::Lines);
        }

        // Dessiner les balles
        for (size_t i = 0; i < balles.size(); ++i) {
            balles[i].draw(window);

            // Créer un texte avec l'indice de la balle
            sf::Text indexText;
            indexText.setFont(font);
            indexText.setString(std::to_string(i)); // Convertir l'indice en string
            indexText.setCharacterSize(30); // Taille du texte
            indexText.setFillColor(sf::Color::Red); // Couleur du texte
            indexText.setPosition(balles[i].getPosition().x, balles[i].getPosition().y); // Positionner le texte près de la balle

            // Afficher l'indice de la balle
            window.draw(indexText);
        }

    box.draw(window);

        // Ajoutez cette partie dans votre boucle principale
        // après avoir dessiné la grille mais avant de dessiner les balles

        // Pour chaque cellule de la grille, calculer et afficher son indice de hash
        // Assurez-vous que la police est bien chargée
        if (fontLoaded) {
            // Calculer le nombre de cellules dans chaque direction
            int gridSizeX = window.getSize().x / smoothingRadius;
            int gridSizeY = window.getSize().y / smoothingRadius;

            // Pour chaque cellule de la grille
            for (int y = 0; y < gridSizeY; y++) {
                for (int x = 0; x < gridSizeX; x++) {
                    // Calculer les coordonnées du coin supérieur gauche de la cellule
                    float cellX = x * smoothingRadius;
                    float cellY = y * smoothingRadius;

                    // Calculer l'indice de hash de cette cellule
                    unsigned int cellHash = hashCell(x, y);
                    unsigned int cellKey = getKeyFromHash(cellHash, numParticles);

                    // Créer un texte pour afficher l'indice
                    sf::Text cellText;
                    cellText.setFont(font);
                    cellText.setString(std::to_string(cellKey));
                    cellText.setCharacterSize(20); // Taille plus petite pour tenir dans la cellule
                    cellText.setFillColor(sf::Color::Green); // Couleur différente pour distinguer

                    // Positionner le texte dans le coin supérieur gauche de la cellule
                    // avec un petit décalage pour la lisibilité
                    cellText.setPosition(cellX + 5, cellY + 5);

                    // Afficher le texte
                    window.draw(cellText);
                }
            }
        }
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
