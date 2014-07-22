#pragma once
#include <random>

typedef uint8_t block;

const int CHUNK_SIZE = 16;
const int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
const int WORLD_HEIGHT = 256;

inline uint64_t chunk_hash(int x, int y, int z) {
	uint64_t hash = 0;
	for (size_t i = 0; i < 21; ++i) {
		hash += (z & 1) + ((x & 1) << 1) + ((y & 1) << 2);
		x >>= 1;
		y >>= 1;
		z >>= 1;
	}
	return hash;
}

struct World {
	World();
	void init(uint64_t seed);
	void render();

	std::mt19937_64 rng;
	uint64_t seed;
};


