#include "../include/Start.h"
#include "../include/Balle.h"
#include "../include/Box.h"
#include <cmath>
#include <iostream>
#include <vector>

void Start(std::vector<Balle> &balles, int numParticles, float ballRadius,
           float spacing, Box &box) {
  // Dimensions de la grille
  int particlesPerRow = std::sqrt(numParticles);
  int particlesPerCol = (numParticles + particlesPerRow - 1) / particlesPerRow;

  // Calcul de l'espacement réel entre les balles (en tenant compte de leur
  // rayon)
  float gridSpacing = ballRadius * 2 + spacing;

  // Plages de coordonnées
  float left, right, up, down;
  box.getBounds(&left, &right, &up, &down);
  float maxx = right - 2 * ballRadius;
  float minx = left + 2 * ballRadius;

  float maxy = down - 2 * ballRadius;
  float miny = up + 2 * ballRadius;

  // Calcul des espacements dans les limites données
  float rangeX = maxx - minx;
  float rangeY = maxy - miny;

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
    float x = minx + offsetX + (col * gridSpacing); // Espacement en x centré
    float y = miny + offsetY + (row * gridSpacing); // Espacement en y centré

    // Vérifie si les balles ne sortent pas des limites
    if (x + ballRadius > maxx)
      x = maxx - ballRadius;
    if (y + ballRadius > maxy)
      y = maxy- ballRadius;

    balles.emplace_back(ballRadius, sf::Vector2f(x, y), sf::Vector2f(0, 0));
  }
}
