#include "Start.h"
#include "Balle.h"
#include <cmath>
#include <iostream>
#include <vector>

void Start(std::vector<Balle> &balles, int numParticles, float ballRadius,
           float spacing) {
  // Dimensions de la grille
  int particlesPerRow = std::sqrt(numParticles);
  int particlesPerCol = (numParticles + particlesPerRow - 1) / particlesPerRow;

  // Calcul de l'espacement réel entre les balles (en tenant compte de leur
  // rayon)
  float gridSpacing = ballRadius * 2 + spacing;

  // Plages de coordonnées
  float minX = 100.f;
  float maxX = 700.f;
  float minY = 100.f;
  float maxY = 500.f;

  // Calcul des espacements dans les limites données
  float rangeX = maxX - minX;
  float rangeY = maxY - minY;

  // Calcul du décalage pour centrer la grille
  float offsetX = (rangeX - (particlesPerRow - 1) * gridSpacing) / 2.f;
  float offsetY = (rangeY - (particlesPerCol - 1) * gridSpacing) / 2.f;

  // Créer et positionner les balles
  balles.clear();
  for (int i = 0; i < numParticles; ++i) {
    // Calcul des indices de ligne et de colonne dans la grille
    int row = i / particlesPerRow;
    int col = i % particlesPerRow;

    // Position des balles dans la boîte, avec centrage
    float x = minX + offsetX + (col * gridSpacing); // Espacement en x centré
    float y = minY + offsetY + (row * gridSpacing); // Espacement en y centré

    // Vérifie si les balles ne sortent pas des limites
    if (x + ballRadius > maxX)
      x = maxX - ballRadius;
    if (y + ballRadius > maxY)
      y = maxY - ballRadius;

    balles.emplace_back(ballRadius, sf::Vector2f(x, y), sf::Vector2f(0, 0));
  }
}
