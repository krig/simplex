#pragma once
#include "glm/gtc/noise.hpp"
#include "rand.hpp"

// support tools for chunk generation (like noise functions and so on)

// spatially localized hash function for chunks
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
