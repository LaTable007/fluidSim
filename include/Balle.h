#ifndef BALLE_HPP
#define BALLE_HPP

#include <SFML/Graphics.hpp>

class Balle {
private:
  sf::CircleShape shape; // Forme graphique de la balle
  sf::Vector2f velocity;
  sf::Vector2f predPosition;
  float radius;
  sf::Vector2f position;
  float density;

public:
  // Constructeur
  Balle(float radius, sf::Vector2f startPos, sf::Vector2f startVelocity);

  // Méthode pour mettre à jour la position
  void update(float dt);

  // Méthode pour dessiner la balle
  void draw(sf::RenderWindow &window);

  void updateColor(sf::Uint8* r, sf::Uint8* g, sf::Uint8* b);

  float smoothingKernel(float radius, float dst);

  float smoothingKernelDiravative(float radius, float dst);

  float calculateDensity(std::vector<Balle> &balles, float smoothingRadius);

  sf::Vector2f calculatePressureForce(
    const std::vector<Balle>& balles,
    int particleIndex,
    float smoothingRadius,
    float mass,
    float targetDensity,
    float pressureMultiplier,
    const std::vector<std::pair<unsigned int, int>>& spatialLookup,
    const std::vector<unsigned int>& startIndices,
    int numParticles
);

  float convertDensityToPressure(float density, float targetDensity, float pressureMultiplier);

  // Méthode pour mettre à jour la densité
    void updateDensity(
      const std::vector<Balle>& balles,
      float smoothingRadius,
      int index,
      float mass,
      const std::vector<std::pair<unsigned int, int>>& spatialLookup,
      const std::vector<unsigned int>& startIndices,
      int numParticles
  );
  // Getter pour la densité
  float getDensity() const { return density; }

  float calculateSharedPressure(float densityA, float densityB, float targetDensity, float pressureMultiplier);



  // Getter pour la position
  sf::Vector2f getPosition() const;

  sf::Vector2f getVelocity() const; // Nouvelle méthode

  sf::Vector2f getPredPosition() const;


  void setVelocity(sf::Vector2f vel);
  void setPosition(sf::Vector2f pos);

  // Ajouter la méthode getRadius pour obtenir le rayon de la balle
  float getRadius() const;

  void setRadius(float radius);

  void setPredPosition(sf::Vector2f pos);
};

void updateSpatialLookup(std::vector<Balle> &balles, float radius, int numParticles, std::vector<std::pair<unsigned int, int>> &spatialLookup, std::vector<unsigned int> &startIndices);

std::pair<int, int> positionToCellCoord(sf::Vector2f point, float radius);

unsigned int hashCell(int cellX, int cellY);

unsigned int getKeyFromHash(unsigned int hash, int numParticle);

void foreachPointInRadius(
    sf::Vector2f samplePoint,
    float radius,
    const std::vector<Balle>& balles,
    const std::vector<std::pair<unsigned int, int>>& spatialLookup,
    const std::vector<unsigned int>& startIndices,
    int numParticles,
    std::function<void(int)> callback);

#endif
