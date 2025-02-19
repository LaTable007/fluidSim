#include "StartRandom.h"
#include "Balle.h"
#include "Box.h"
#include <random>

float generateRandomFloat(float min, float max) {
  // Crée un générateur de nombres aléatoires
  std::random_device rd;  // Génère une graine unique
  std::mt19937 gen(rd()); // Générateur Mersenne Twister
  std::uniform_real_distribution<float> dist(min, max); // Distribution uniforme

  return dist(gen);
}

void startRandom(std::vector<Balle> &balles, int numberParticles,
                 float radius, Box &box) {
  float left, right, up, down;
  box.getBounds(&left, &right, &up, &down);
  for (int i = 0; i < numberParticles; i++) {
    float maxx = right - 2 * radius;
    float minx = left + 2 * radius;
    float x = generateRandomFloat(minx, maxx);

    float maxy = down - 2 * radius;
    float miny = up + 2 * radius;
    float y = generateRandomFloat(miny, maxy);
    balles.emplace_back(radius, sf::Vector2f(x, y), sf::Vector2f(generateRandomFloat(0, 400), generateRandomFloat(0, 400)));
  };
};
