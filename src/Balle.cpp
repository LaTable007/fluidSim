#include "../include/Balle.h"
#include <SFML/Graphics.hpp>
#include <iostream>

// Constructeur
Balle::Balle(float radius, sf::Vector2f startPos, sf::Vector2f startVelocity)
    : shape(radius), velocity(startVelocity) {
  shape.setPosition(startPos);
  shape.setFillColor(sf::Color::Red); // Couleur rouge par défaut
}


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

float Balle::smoothingKernel(float radius, float dst) {
    if (dst >= radius) {return 0;}

    float volume = (3.14159265359f * pow(radius, 4)) / 6;
    return (radius - dst) * (radius - dst) / volume;
}

float Balle::smoothingKernelDiravative(float radius, float dst) {
    if (dst >= radius) {return 0;}

    float scale = 12.0f / (3.14159265359f * pow(radius, 4));
    return (dst - radius) * scale;
}

float Balle::calculateDensity(std::vector<Balle> &balles, float smoothingRadius) {
    float density = 0.0f;
    float mass = 1.0f;
    sf::Vector2f position = shape.getPosition();
    for (auto &balle : balles) {
        sf::Vector2f samplePoint = balle.getPosition();
        sf::Vector2f dst = (position - samplePoint);
        float dstsqrt = sqrt(dst.x*dst.x + dst.y*dst.y);
        if (dstsqrt >= smoothingRadius) {continue;}
        float influence = smoothingKernel(smoothingRadius, dstsqrt);
        density += mass * influence;
    }
    return density;
}

sf::Vector2f Balle::calculatePressureForce(
    const std::vector<Balle>& balles,
    int particleIndex,
    float smoothingRadius,
    float mass,
    float targetDensity,
    float pressureMultiplier,
    const std::vector<std::pair<unsigned int, int>>& spatialLookup,
    const std::vector<unsigned int>& startIndices,
    int numParticles
) {
    sf::Vector2f pressureForce(0.0f, 0.0f);
    sf::Vector2f pos = getPredPosition();

    foreachPointInRadius(
        pos,                // Point central
        smoothingRadius,    // Rayon de recherche
        balles,             // Vecteur des particules
        spatialLookup,      // Structure spatiale
        startIndices,       // Indices de départ
        numParticles,       // Nombre total de particules
        [&](int neighborIdx) { // Callback
            if (particleIndex != neighborIdx) {
                sf::Vector2f dst = balles[neighborIdx].getPredPosition() - pos;
                float dstsqrt = std::sqrt(dst.x * dst.x + dst.y * dst.y);

                if (dstsqrt > 0) {
                    sf::Vector2f dir = dst / dstsqrt;
                    float slope = smoothingKernelDiravative(smoothingRadius, dstsqrt);

                    float sharedPressure = calculateSharedPressure(
                        density,
                        balles[neighborIdx].getDensity(),
                        targetDensity,
                        pressureMultiplier
                    );

                    pressureForce += sharedPressure * dir * slope * mass / density;
                }
            }
        }
    );

    return pressureForce;
}

sf::Vector2f Balle::calculateViscosityForce(
    const std::vector<Balle>& balles,
    int particleIndex,
    float smoothingRadius,
    float viscosity,
    const std::vector<std::pair<unsigned int, int>>& spatialLookup,
    const std::vector<unsigned int>& startIndices,
    int numParticles
) {
    sf::Vector2f viscosityForce(0.0f, 0.0f);
    sf::Vector2f pos = getPredPosition();

    foreachPointInRadius(
        pos,                // Point central
        smoothingRadius,    // Rayon de recherche
        balles,             // Vecteur des particules
        spatialLookup,      // Structure spatiale
        startIndices,       // Indices de départ
        numParticles,       // Nombre total de particules
        [&](int neighborIdx) { // Callback
            if (particleIndex != neighborIdx) {
                sf::Vector2f dst = balles[neighborIdx].getPredPosition() - pos;
                float dstsqrt = std::sqrt(dst.x * dst.x + dst.y * dst.y);

                if (dstsqrt > 0) {
                    sf::Vector2f dir = dst / dstsqrt;
                    float slope = viscositySmoothingKernel(smoothingRadius, dstsqrt);

                    viscosityForce += (balles[neighborIdx].getVelocity() - velocity) * slope;
                }
            }
        }
    );

    return viscosityForce * viscosity;
}

float Balle::viscositySmoothingKernel(float smoothingRadius, float dstsqrt) {
    if (dstsqrt >= radius) {return 0;}
    float volume = 3.14159265359f * pow(smoothingRadius, 8) / 4;
    float value = smoothingRadius * smoothingRadius - dstsqrt * dstsqrt;
    return value * value * value / volume;
}


float Balle::convertDensityToPressure(float density, float targetDensity, float pressureMultiplier) {
    float densityError = targetDensity - density;
    float pressure = pressureMultiplier * densityError;
    return pressure;
}

void Balle::updateDensity(
    const std::vector<Balle>& balles,
    float smoothingRadius,
    int index,
    float mass,
    const std::vector<std::pair<unsigned int, int>>& spatialLookup,
    const std::vector<unsigned int>& startIndices,
    int numParticles
) {
    density = 0.0f;
    sf::Vector2f pos = getPredPosition();

    foreachPointInRadius(
        pos,
        smoothingRadius,
        balles,
        spatialLookup,
        startIndices,
        numParticles,
        [&](int neighborIdx) {
            sf::Vector2f diff = pos - balles[neighborIdx].getPredPosition();
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            density += mass * smoothingKernel(smoothingRadius, dist);
        }
    );
}


float Balle::calculateSharedPressure(float densityA, float densityB, float targetDensity, float pressureMultiplier) {
    float pressureA = convertDensityToPressure(densityA, targetDensity, pressureMultiplier);
    float pressureB = convertDensityToPressure(densityB, targetDensity, pressureMultiplier);
    return (pressureA + pressureB) / 2;
}


void Balle::setRadius(float radius) {
    shape.setRadius(radius);
    shape.setOrigin(radius, radius); // Update origin to keep the ball centered
}

void updateSpatialLookup(std::vector<Balle> &balles, float radius, int numParticles,
                         std::vector<std::pair<unsigned int, int>> &spatialLookup, // Utilisation d'un vecteur de paires
                         std::vector<unsigned int> &startIndices) {
    // Initialiser startIndices avec INT_MAX
    std::fill(startIndices.begin(), startIndices.end(), INT_MAX);
    spatialLookup.clear();  // Vider la structure de données précédente

    // Remplissage de spatialLookup avec des paires (index, cellKey)
    for (int i = 0; i < numParticles; i++) {
        std::pair<int, int> cell = positionToCellCoord(balles[i].getPosition(), radius);
        int cellX = cell.first;
        int cellY = cell.second;
        unsigned cellKey = getKeyFromHash(hashCell(cellX, cellY), numParticles);
        //std::cout << "Ball " << i << " -> Cell (" << cellX << ", " << cellY << "), Key: " << cellKey << std::endl;

        spatialLookup.push_back(std::make_pair(i, cellKey)); // Ajouter la paire (index, cellKey)
    }

    // Trier spatialLookup par ordre croissant des cellKey
    std::sort(spatialLookup.begin(), spatialLookup.end(),
              [](const auto &a, const auto &b) { return a.second < b.second; });

    // Réinitialiser startIndices
    std::fill(startIndices.begin(), startIndices.end(), INT_MAX);

    // Remplir startIndices en fonction de l'ordre trié
    for (int i=0; i<numParticles; i++) {

        unsigned int key = spatialLookup[i].second;
        if (i == 0 || key != spatialLookup[i - 1].second) {
            startIndices[key] = i;
        }
    }
}

std::pair<int, int> positionToCellCoord(sf::Vector2f point, float radius) {
    int cellX = static_cast<int>(std::floor((point.x / radius)));
    int cellY = static_cast<int>(std::floor((point.y / radius)));
    return {cellX, cellY};
}

unsigned int hashCell(int cellX, int cellY) {
    unsigned int a = cellX * 15823;
    unsigned int b = cellY * 9737333;
    return a + b;
}

unsigned int getKeyFromHash(unsigned int hash, int numParticle) {

    return hash % numParticle;
}

void foreachPointInRadius(
    sf::Vector2f samplePoint,
    float radius,
    const std::vector<Balle>& balles,
    const std::vector<std::pair<unsigned int, int>>& spatialLookup,
    const std::vector<unsigned int>& startIndices,
    int numParticles,
    std::function<void(int)> callback)
{
    const auto [centreX, centreY] = positionToCellCoord(samplePoint, radius);
    const float sqrRadius = radius * radius;

    const std::vector<std::pair<int, int>> cellOffsets = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1,  0}, {0,  0}, {1,  0},
        {-1,  1}, {0,  1}, {1,  1}
    };
    for(const auto& [dx, dy] : cellOffsets) {
        const int cellX = centreX + dx;
        const int cellY = centreY + dy;

        // Calcul de la clé de la cellule
        const unsigned int h = hashCell(cellX, cellY);
        const unsigned int key = getKeyFromHash(h, numParticles);

        // Vérification des limites
        if(key >= startIndices.size()) continue;
        const unsigned int startIdx = startIndices[key];
        if(startIdx == INT_MAX) continue;

        // Parcours des particules dans la cellule
        for(size_t i = startIdx; i < spatialLookup.size(); ++i) {
            if(spatialLookup[i].second != key) break;

            const int particleIdx = spatialLookup[i].first;
            const auto& pos = balles[particleIdx].getPosition();
            const float sqrDst =
                (pos.x - samplePoint.x) * (pos.x - samplePoint.x) +
                (pos.y - samplePoint.y) * (pos.y - samplePoint.y);

            if(sqrDst <= sqrRadius) {
                callback(particleIdx);
            }
        }
    }
}

// Récupérer la position
sf::Vector2f Balle::getPosition() const { return shape.getPosition(); }

sf::Vector2f Balle::getVelocity() const { return velocity; }

sf::Vector2f Balle::getPredPosition() const { return predPosition; }

void Balle::setVelocity(sf::Vector2f vel) { velocity = vel; }

void Balle::setPosition(sf::Vector2f pos) { shape.setPosition(pos); }

void Balle::setPredPosition(sf::Vector2f pos) { predPosition = pos; }
// Ajout de la méthode getRadius
float Balle::getRadius() const { return shape.getRadius(); }