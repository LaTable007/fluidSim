#include "Balle.h"
#include <SFML/Graphics.hpp>

// Constructeur
Balle::Balle(float radius, sf::Vector2f startPos, sf::Vector2f startVelocity)
    : shape(radius), velocity(startVelocity) {
  shape.setPosition(startPos);
  shape.setFillColor(sf::Color::Red); // Couleur rouge par défaut
}

int layers = 10; // Nombre de couches pour l'effet de flou
sf::Color blurColor = sf::Color(173, 216, 230, 50);

// Mise à jour manuelle de la position
void Balle::update(float dt) {
  sf::Vector2f currentPos =
      shape.getPosition(); // Récupérer la position actuelle
  sf::Vector2f newPos =
      currentPos + (velocity * dt); // Calculer la nouvelle position
  shape.setPosition(newPos);        // Mettre à jour la position de la balle
}

// Dessiner la balle dans la fenêtre
void Balle::draw(sf::RenderWindow &window) {
  float radius = shape.getRadius();
  sf::Vector2f position = shape.getPosition();

  // Dessiner les couches de flou
  for (int i = 0; i < layers; ++i) {
    sf::CircleShape circle(radius +
                           i * 5); // Rayon plus grand pour chaque couche
    circle.setOrigin(circle.getRadius(), circle.getRadius());
    circle.setPosition(position);
    circle.setFillColor(blurColor); // Couleur semi-transparente
    window.draw(circle);
  }

  // Dessiner la balle principale
  // window.draw(shape);
}

// Récupérer la position
sf::Vector2f Balle::getPosition() const { return shape.getPosition(); }

sf::Vector2f Balle::getVelocity() const { return velocity; }

void Balle::setVelocity(sf::Vector2f vel) { velocity = vel; }

// Ajout de la méthode getRadius
float Balle::getRadius() const { return shape.getRadius(); }
