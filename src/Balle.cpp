#include "../include/Balle.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <climits>

// Constructeur
Balle::Balle(float radius, sf::Vector2f startPos, sf::Vector2f startVelocity)
: shape(radius), velocity(startVelocity), density(0.0f)
{
    shape.setPosition(startPos);
    shape.setFillColor(sf::Color::Red);
    shape.setOrigin(radius, radius);
}

// Mise à jour de la position
void Balle::update(float dt) {
    sf::Vector2f currentPos = shape.getPosition();
    sf::Vector2f newPos = currentPos + (velocity * dt);
    shape.setPosition(newPos);
}

// Dessin avec couleur par défaut (via surcharge)
void Balle::draw(sf::RenderWindow &window) {
    draw(window, sf::Color::Transparent);
}

// Surcharge de draw avec couleur d'override
void Balle::draw(sf::RenderWindow &window, const sf::Color &overrideColor) {
    float radius = shape.getRadius();
    sf::Vector2f position = shape.getPosition();
    sf::CircleShape circle(radius);
    circle.setOrigin(radius, radius);
    circle.setPosition(position);

    if (overrideColor != sf::Color::Transparent) {
        circle.setFillColor(overrideColor);
    } else {
        sf::Uint8 r, g, b;
        updateColor(&r, &g, &b);
        circle.setFillColor(sf::Color(r, g, b));
    }
    window.draw(circle);
}

void Balle::updateColor(sf::Uint8* r, sf::Uint8* g, sf::Uint8* b) {
    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    float minSpeed = 0.f;
    float maxSpeed = 500.f;
    if (speed < minSpeed) speed = minSpeed;
    if (speed > maxSpeed) speed = maxSpeed;
    float factor = (speed - minSpeed) / (maxSpeed - minSpeed);
    struct ControlPoint {
        float t;
        sf::Uint8 r, g, b;
    };
    static const ControlPoint controlPoints[] = {
        {0.0f, 12, 7, 134},
        {0.25f, 73, 27, 167},
        {0.5f, 145, 55, 122},
        {0.75f, 210, 101, 70},
        {1.0f, 248, 246, 33}
    };
    const int nPoints = sizeof(controlPoints) / sizeof(ControlPoint);
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
    for (int i = 0; i < nPoints - 1; ++i) {
        if (factor >= controlPoints[i].t && factor < controlPoints[i+1].t) {
            float localFactor = (factor - controlPoints[i].t) / (controlPoints[i+1].t - controlPoints[i].t);
            *r = static_cast<sf::Uint8>(controlPoints[i].r + localFactor * (controlPoints[i+1].r - controlPoints[i].r));
            *g = static_cast<sf::Uint8>(controlPoints[i].g + localFactor * (controlPoints[i+1].g - controlPoints[i].g));
            *b = static_cast<sf::Uint8>(controlPoints[i].b + localFactor * (controlPoints[i+1].b - controlPoints[i].b));
            return;
        }
    }
}

float Balle::smoothingKernel(float radius, float dst) {
    if (dst >= radius) return 0;
    float volume = (3.14159265359f * std::pow(radius, 4)) / 6;
    return (radius - dst) * (radius - dst) / volume;
}

float Balle::smoothingKernelDiravative(float radius, float dst) {
    if (dst >= radius) return 0;
    float scale = 12.0f / (3.14159265359f * std::pow(radius, 4));
    return (dst - radius) * scale;
}

float Balle::calculateDensity(std::vector<Balle> &balles, float smoothingRadius) {
    float density = 0.0f;
    float mass = 1.0f;
    sf::Vector2f position = shape.getPosition();
    for (auto &balle : balles) {
        sf::Vector2f samplePoint = balle.getPosition();
        sf::Vector2f dst = position - samplePoint;
        float dstsqrt = std::sqrt(dst.x * dst.x + dst.y * dst.y);
        if (dstsqrt >= smoothingRadius) continue;
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
    // Utilisation d'un vecteur temporaire pour affectedBalles (non utilisé ici)
    std::vector<int> tmpAffected;
    foreachPointInRadius(
        pos,
        smoothingRadius,
        balles,
        spatialLookup,
        startIndices,
        numParticles,
        [&](int neighborIdx) {
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
        },
        tmpAffected
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
    std::vector<int> tmpAffected;
    foreachPointInRadius(
        pos,
        smoothingRadius,
        balles,
        spatialLookup,
        startIndices,
        numParticles,
        [&](int neighborIdx) {
            if (particleIndex != neighborIdx) {
                sf::Vector2f dst = balles[neighborIdx].getPredPosition() - pos;
                float dstsqrt = std::sqrt(dst.x * dst.x + dst.y * dst.y);
                if (dstsqrt > 0) {
                    sf::Vector2f dir = dst / dstsqrt;
                    float slope = viscositySmoothingKernel(smoothingRadius, dstsqrt);
                    viscosityForce += (balles[neighborIdx].getVelocity() - velocity) * slope;
                }
            }
        },
        tmpAffected
    );
    return viscosityForce * viscosity;
}

float Balle::viscositySmoothingKernel(float smoothingRadius, float dstsqrt) {
    if (dstsqrt >= smoothingRadius) return 0;
    float volume = 3.14159265359f * std::pow(smoothingRadius, 8) / 4;
    float value = smoothingRadius * smoothingRadius - dstsqrt * dstsqrt;
    return std::pow(value, 3) / volume;
}

float Balle::convertDensityToPressure(float density, float targetDensity, float pressureMultiplier) {
    float densityError = targetDensity - density;
    return pressureMultiplier * densityError;
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
    std::vector<int> tmpAffected;
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
        },
        tmpAffected
    );
}

float Balle::calculateSharedPressure(float densityA, float densityB, float targetDensity, float pressureMultiplier) {
    float pressureA = convertDensityToPressure(densityA, targetDensity, pressureMultiplier);
    float pressureB = convertDensityToPressure(densityB, targetDensity, pressureMultiplier);
    return (pressureA + pressureB) / 2;
}

void Balle::setRadius(float radius) {
    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
}

// Fonctions utilitaires pour la spatialisation

std::pair<int, int> positionToCellCoord(sf::Vector2f point, float radius) {
    int cellX = static_cast<int>(std::floor(point.x / radius));
    int cellY = static_cast<int>(std::floor(point.y / radius));
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

void updateSpatialLookup(std::vector<Balle> &balles, float radius, int numParticles,
    std::vector<std::pair<unsigned int, int>> &spatialLookup,
    std::vector<unsigned int> &startIndices)
{
    std::fill(startIndices.begin(), startIndices.end(), INT_MAX);
    spatialLookup.clear();
    for (int i = 0; i < numParticles; i++) {
        std::pair<int, int> cell = positionToCellCoord(balles[i].getPosition(), radius);
        int cellX = cell.first;
        int cellY = cell.second;
        unsigned cellKey = getKeyFromHash(hashCell(cellX, cellY), numParticles);
        spatialLookup.push_back(std::make_pair(i, cellKey));
    }
    std::sort(spatialLookup.begin(), spatialLookup.end(),
              [](const auto &a, const auto &b) { return a.second < b.second; });
    std::fill(startIndices.begin(), startIndices.end(), INT_MAX);
    for (int i = 0; i < numParticles; i++) {
        unsigned int key = spatialLookup[i].second;
        if (i == 0 || key != spatialLookup[i - 1].second) {
            startIndices[key] = i;
        }
    }
}

void foreachPointInRadius(
    sf::Vector2f samplePoint,
    float radius,
    const std::vector<Balle>& balles,
    const std::vector<std::pair<unsigned int, int>>& spatialLookup,
    const std::vector<unsigned int>& startIndices,
    int numParticles,
    std::function<void(int)> callback,
    std::vector<int>& affectedBalles)
{
    const auto [centreX, centreY] = positionToCellCoord(samplePoint, radius);
    const float sqrRadius = radius * radius;
    const std::vector<std::pair<int, int>> cellOffsets = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1,  0}, {0,  0}, {1,  0},
        {-1,  1}, {0,  1}, {1,  1}
    };
    for (const auto& [dx, dy] : cellOffsets) {
        const int cellX = centreX + dx;
        const int cellY = centreY + dy;
        const unsigned int h = hashCell(cellX, cellY);
        const unsigned int key = getKeyFromHash(h, numParticles);
        if (key >= startIndices.size()) continue;
        const unsigned int startIdx = startIndices[key];
        if (startIdx == INT_MAX) continue;
        for (size_t i = startIdx; i < spatialLookup.size(); ++i) {
            if (spatialLookup[i].second != key)
                break;
            const int particleIdx = spatialLookup[i].first;
            const auto& pos = balles[particleIdx].getPosition();
            float sqrDst = (pos.x - samplePoint.x) * (pos.x - samplePoint.x) +
                           (pos.y - samplePoint.y) * (pos.y - samplePoint.y);
            affectedBalles.push_back(particleIdx);
            if (sqrDst <= sqrRadius) {
                callback(particleIdx);

            }
        }
    }
}

sf::Vector2f Balle::getPosition() const { return shape.getPosition(); }
sf::Vector2f Balle::getVelocity() const { return velocity; }
sf::Vector2f Balle::getPredPosition() const { return predPosition; }
void Balle::setVelocity(sf::Vector2f vel) { velocity = vel; }
void Balle::setPosition(sf::Vector2f pos) { shape.setPosition(pos); }
void Balle::setPredPosition(sf::Vector2f pos) { predPosition = pos; }
float Balle::getRadius() const { return shape.getRadius(); }
float Balle::getDensity() const { return density; }
