#include "common.hpp"
#include "render.hpp"
#include "assets.hpp"
#include "rand.hpp"
#include "world.hpp"
#include "glm/gtc/noise.hpp"

World::World() {
}

void World::init(uint64_t seed) {
	this->seed = seed;
	rng.seed(seed);
}

void World::render() {
}

