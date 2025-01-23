#include "StartRandom.h"
#include "Balle.h"
#include <random>

float generateRandomFloat(float min, float max) {
  // Crée un générateur de nombres aléatoires
  std::random_device rd;  // Génère une graine unique
  std::mt19937 gen(rd()); // Générateur Mersenne Twister
  std::uniform_real_distribution<float> dist(min, max); // Distribution uniforme

  return dist(gen);
}

void startRandom(std::vector<Balle> &balles, int numberParticles,
                 float radius) {
  for (int i = 0; i < numberParticles; i++) {
    float minx = 000;
    float maxx = 800 - 2 * radius;
    float x = generateRandomFloat(minx, maxx);

    float miny = 000;
    float maxy = 600 - 2 * radius;
    float y = generateRandomFloat(miny, maxy);
    balles.emplace_back(radius, sf::Vector2f(x, y), sf::Vector2f(0, 0));
  };
};
