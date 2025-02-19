#ifndef START_RANDOM_HPP
#define START_RANDOM_HPP

#include "Balle.h"
#include "StartRandom.h"
#include "Box.h"
#include <random>

float generateRandomFloat(float min, float max);

void startRandom(std::vector<Balle> &balles, int numberParticles, float radius, Box &box);

#endif
