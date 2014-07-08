#pragma once

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
const int INITIAL_CHUNK_CACHE_SIZE_MB = 64;

// for 16^3 chunks and 1 byte/voxel = 16k chunks in cache if 64MB chunk cache
const int INITIAL_CHUNK_CACHE_SIZE = (INITIAL_CHUNK_CACHE_SIZE_MB * 1024 * 1024)
	/ (sizeof(block) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);

struct chunk {
	uint32_t id;
	int16_t x, y, z;
	uint16_t flags;
	// voxel data for chunk
	block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
};

// the chunk cache has a maximum size
// and background loader threads
struct chunk_cache {
	chunk cache[INITIAL_CHUNK_CACHE_SIZE];
};

struct world {
	uint64_t seed;
};
