#include "common.hpp"
#include "render.hpp"
#include "assets.hpp"
#include "rand.hpp"
#include "world.hpp"
#include "glm/gtc/noise.hpp"

#define BLOCK_INDEX(x, y, z) (((y) * CHUNK_SIZE * CHUNK_SIZE) + ((z) * CHUNK_SIZE) + x)

enum class Face { Bottom, Top, Back, Front, Left, Right };

void generate_face(Face face, int x, int y, int z) {}

// TODO: tesselation can generate multiple meshes,
// one for each type of block
Mesh* chunk::tesselate() {
	for (int y = 0; y < CHUNK_SIZE; ++y) {
		for (int z = 0; z < CHUNK_SIZE; ++z) {
			for (int x = 0; x < CHUNK_SIZE; ++x) {
				uint8_t type = blocks[BLOCK_INDEX(x, y, z)];
				if (type == 0)
					continue;
				uint8_t bottom = (y == 0) ? 0 : blocks[BLOCK_INDEX(x, y-1, z)];
				uint8_t top    = (y == (CHUNK_SIZE-1)) ? 0 : blocks[BLOCK_INDEX(x, y+1, z)];
				uint8_t back   = (z == 0) ? 0 : blocks[BLOCK_INDEX(x, y, z-1)];
				uint8_t front  = (z == (CHUNK_SIZE-1)) ? 0 : blocks[BLOCK_INDEX(x, y, z+1)];
				uint8_t left   = (x == 0) ? 0 : blocks[BLOCK_INDEX(x-1, y, z)];
				uint8_t right  = (x == (CHUNK_SIZE-1)) ? 0 : blocks[BLOCK_INDEX(x+1, y, z)];
				if (bottom == 0)
					generate_face(Face::Bottom, x, y, z);
				if (top == 0)
					generate_face(Face::Top, x, y, z);
				if (back == 0)
					generate_face(Face::Back, x, y, z);
				if (front == 0)
					generate_face(Face::Front, x, y, z);
				if (left == 0)
					generate_face(Face::Left, x, y, z);
				if (right == 0)
					generate_face(Face::Right, x, y, z);
			}
		}
	}

	return nullptr;
}

World::World() {
}

void World::init(uint64_t seed) {
	this->seed = seed;
	rng.seed(seed);

	//chunks.reserve(16*10*10);
	//for (int y = 0; y < 16; ++y) {
	//	for (int z = -10; z < 10; ++z) {
	//		for (int x = -10; x < 10; ++x) {
	//			chunks.emplace_back(generate_chunk(x, y, z, seed));
	//		}
	//	}
	//}
}

void World::generate(int x, int z) {
	for (int y = 0; y < 16; ++y) {
		uint64_t hash = chunk_hash(x, y, z);
		if (chunks.find(hash) == chunks.end()) {
			chunks[hash] = generate_chunk(x, y, z, seed);
		}
	}
}

void World::render() {
	for (Mesh* m : meshes) {
		bind_vao(m->vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glDrawArrays(GL_TRIANGLES, 0, m->ntriangles);
	}
}

float solidity(float x, float y, float z) {
	float gradient0 = y * (1.f/256.f);
	float simplex0 = glm::simplex(vec3(x, y, z));
	return gradient0 * simplex0;
}


chunk generate_chunk(int x, int y, int z, uint64_t seed) {
	chunk ch;
	ch.x = x;
	ch.y = y;
	ch.z = z;
	ch.mesh = nullptr;

	for (int by = 0; by < CHUNK_SIZE; ++by) {
		for (int bz = 0; bz < CHUNK_SIZE; ++bz) {
			for (int bx = 0; bx < CHUNK_SIZE; ++bx) {
				float v = solidity((float)(x + bx), (float)(y + by), (float)(z + bz));
				uint8_t block = (v > 0.5f) ? 1 : 0;
				ch.blocks[(by * CHUNK_SIZE * CHUNK_SIZE) + (bz * CHUNK_SIZE) + bx] = block;
			}
		}
	}

	return ch;
}
