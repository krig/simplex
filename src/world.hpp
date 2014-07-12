#pragma once
#include <random>

struct thing {
	glm::mat4 pos;

	thing() {
	}

	void move(const glm::vec3& by) {
		pos = glm::translate(pos, by);
	}

	void rotate(float angle, const glm::vec3& axis) {
		pos = glm::rotate(pos, angle, axis);
	}

	void look_at(const glm::vec3& from, const glm::vec3& to, const glm::vec3& up = glm::vec3(0.f, 1.f, 0.f)) {
		pos = glm::lookAt(from, to, up);
	}
};

typedef uint8_t block;

const int CHUNK_SIZE = 16;

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

struct chunk {
	int x, y, z;
	Mesh* mesh;
	// voxel data for chunk
	block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];

	Mesh* tesselate();
};

struct World {
	World();
	void init(uint64_t seed);
	void render();
	// generate chunk column at (x, z)
	void generate(int x, int z);

	std::mt19937_64 rng;
	uint64_t seed;
	std::map<uint64_t, chunk> chunks;
	std::vector<Mesh*> meshes;

};

chunk generate_chunk(int x, int y, int z, uint64_t seed);

