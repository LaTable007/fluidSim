#ifndef BALLE_H
#define BALLE_H

#include <SFML/Graphics.hpp>
#include <functional>
#include <utility>
#include <vector>

class Balle {
public:
  Balle(float radius, sf::Vector2f startPos, sf::Vector2f startVelocity);

  void update(float dt);
  void draw(sf::RenderWindow &window);
  void draw(sf::RenderWindow &window, const sf::Color &overrideColor);

  void updateColor(sf::Uint8 *r, sf::Uint8 *g, sf::Uint8 *b);

  float smoothingKernel(float radius, float dst);
  float smoothingKernelDiravative(float radius, float dst);

  float calculateDensity(std::vector<Balle> &balles, float smoothingRadius);
  sf::Vector2f calculatePressureForce(
      const std::vector<Balle> &balles, int particleIndex,
      float smoothingRadius, float mass, float targetDensity,
      float pressureMultiplier,
      const std::vector<std::pair<unsigned int, int>> &spatialLookup,
      const std::vector<unsigned int> &startIndices, int numParticles);
  sf::Vector2f calculateViscosityForce(
      const std::vector<Balle> &balles, int particleIndex,
      float smoothingRadius, float viscosity,
      const std::vector<std::pair<unsigned int, int>> &spatialLookup,
      const std::vector<unsigned int> &startIndices, int numParticles);

  float viscositySmoothingKernel(float smoothingRadius, float dstsqrt);
  float convertDensityToPressure(float density, float targetDensity,
                                 float pressureMultiplier);
  void
  updateDensity(const std::vector<Balle> &balles, float smoothingRadius,
                int index, float mass,
                const std::vector<std::pair<unsigned int, int>> &spatialLookup,
                const std::vector<unsigned int> &startIndices,
                int numParticles);
  float calculateSharedPressure(float densityA, float densityB,
                                float targetDensity, float pressureMultiplier);

  void setRadius(float radius);

  sf::Vector2f getPosition() const;
  sf::Vector2f getVelocity() const;
  sf::Vector2f getPredPosition() const;

  void setVelocity(sf::Vector2f vel);
  void setPosition(sf::Vector2f pos);
  void setPredPosition(sf::Vector2f pos);

  float getRadius() const;
  float getDensity() const;

private:
  sf::CircleShape shape;
  sf::Vector2f velocity;
  sf::Vector2f predPosition;
  float density;
};

// Fonctions utilitaires pour la spatialisation
std::pair<int, int> positionToCellCoord(sf::Vector2f point, float radius);
unsigned int hashCell(int cellX, int cellY);
unsigned int getKeyFromHash(unsigned int hash, int numParticle);
void updateSpatialLookup(
    std::vector<Balle> &balles, float radius, int numParticles,
    std::vector<std::pair<unsigned int, int>> &spatialLookup,
    std::vector<unsigned int> &startIndices);
void foreachPointInRadius(
    sf::Vector2f samplePoint, float radius, const std::vector<Balle> &balles,
    const std::vector<std::pair<unsigned int, int>> &spatialLookup,
    const std::vector<unsigned int> &startIndices, int numParticles,
    std::function<void(int)> callback, std::vector<int> &affectedBalles);

#endif // BALLE_H
