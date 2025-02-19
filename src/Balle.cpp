#include "Balle.h"
#include <SFML/Graphics.hpp>

// Constructeur
Balle::Balle(float radius, sf::Vector2f startPos, sf::Vector2f startVelocity)
    : shape(radius), velocity(startVelocity) {
  shape.setPosition(startPos);
  shape.setFillColor(sf::Color::Red); // Couleur rouge par défaut
}

int layers = 10; // Nombre de couches pour l'effet de flou
sf::Color blurColor = sf::Color(173, 216, 230, 255);

// Mise à jour manuelle de la position
void Balle::update(float dt) {
  sf::Vector2f currentPos =
      shape.getPosition(); // Récupérer la position actuelle
  sf::Vector2f newPos =
      currentPos + (velocity * dt); // Calculer la nouvelle position
  shape.setPosition(newPos);
}

// Dessiner la balle dans la fenêtre
void Balle::draw(sf::RenderWindow &window) {
  float radius = shape.getRadius();
  sf::Vector2f position = shape.getPosition();

  sf::CircleShape circle(radius); // Rayon plus grand pour chaque couche
  circle.setOrigin(circle.getRadius(), circle.getRadius());
  circle.setPosition(position);

  sf::Uint8 r; sf::Uint8 g; sf::Uint8 b;
  updateColor(&r, &g, &b);

  circle.setFillColor(sf::Color(r, g, b)); // Couleur semi-transparente
  window.draw(circle);


  // Dessiner la balle principale
  // window.draw(shape);
}

void Balle::updateColor(sf::Uint8* r, sf::Uint8* g, sf::Uint8* b) {
    // Calcul de la vitesse (module du vecteur vitesse)
    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

    // Définir une plage de vitesse minimale et maximale
    float minSpeed = 0.f;   // Couleur associée à une vitesse faible
    float maxSpeed = 500.f; // Couleur associée à une vitesse élevée

    // Clamp la vitesse
    if (speed < minSpeed) speed = minSpeed;
    if (speed > maxSpeed) speed = maxSpeed;

    // Calcul d'un facteur entre 0 et 1
    float factor = (speed - minSpeed) / (maxSpeed - minSpeed);

    // Définir quelques points de contrôle pour le dégradé plasma
    // Ces valeurs ont été choisies pour se rapprocher de la palette plasma de matplotlib :
    // À 0.0 : un bleu profond, à 1.0 : un jaune éclatant
    struct ControlPoint {
        float t;          // Position (entre 0 et 1)
        sf::Uint8 r, g, b; // Couleur associée
    };

    static const ControlPoint controlPoints[] = {
        {0.0f,  12,   7, 134},  // Bleu profond
        {0.25f, 73,  27, 167},   // Passage vers le violet
        {0.5f, 145,  55, 122},   // Pourpre
        {0.75f, 210, 101,  70},   // Orange
        {1.0f, 248, 246,  33}    // Jaune vif
    };

    const int nPoints = sizeof(controlPoints) / sizeof(ControlPoint);

    // Si le facteur est en-dehors des bornes, on affecte directement la couleur extrême
    if (factor <= controlPoints[0].t) {
        *r = controlPoints[0].r;
        *g = controlPoints[0].g;
        *b = controlPoints[0].b;
        return;
    }
    if (factor >= controlPoints[nPoints - 1].t) {
        *r = controlPoints[nPoints - 1].r;
        *g = controlPoints[nPoints - 1].g;
        *b = controlPoints[nPoints - 1].b;
        return;
    }

    // Recherche de l'intervalle où se trouve factor
    for (int i = 0; i < nPoints - 1; ++i) {
        if (factor >= controlPoints[i].t && factor < controlPoints[i+1].t) {
            // Calcul d'un facteur local dans cet intervalle
            float localFactor = (factor - controlPoints[i].t) / (controlPoints[i+1].t - controlPoints[i].t);

            // Interpolation linéaire entre les couleurs des points de contrôle
            *r = static_cast<sf::Uint8>(controlPoints[i].r + localFactor * (controlPoints[i+1].r - controlPoints[i].r));
            *g = static_cast<sf::Uint8>(controlPoints[i].g + localFactor * (controlPoints[i+1].g - controlPoints[i].g));
            *b = static_cast<sf::Uint8>(controlPoints[i].b + localFactor * (controlPoints[i+1].b - controlPoints[i].b));
            return;
        }
    }
}



// Récupérer la position
sf::Vector2f Balle::getPosition() const { return shape.getPosition(); }

sf::Vector2f Balle::getVelocity() const { return velocity; }

void Balle::setVelocity(sf::Vector2f vel) { velocity = vel; }

void Balle::setPosition(sf::Vector2f pos) { shape.setPosition(pos); }
// Ajout de la méthode getRadius
float Balle::getRadius() const { return shape.getRadius(); }
