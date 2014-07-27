#pragma once
#include "chunksupport.hpp"

struct Block {
	const char* name;
};

// 0 - 255 = index in block_types
// > 255 = chunkblock index (holds per-block state in an array in the chunk)
typedef uint32_t BlockId;

struct SubChunk {
	virtual ~SubChunk() {}
};

struct EmptySubChunk : public SubChunk {
};

struct NormalSubChunk : public SubChunk {
};

// a 16x16x16x? pillar of blocks
// subchunks are 16^3 pieces of a chunk
// subchunk 0 contains 0-15, 1 = 16-31...
struct Chunk {
	SubChunk* subchunks[32];
};

struct World;

struct WorldGenerator {
	World* world;
	util::random_generator rng;
};

struct World {

	Chunk* generate(int x, int z);

	util::seed_t seed;
	dict<uint64_t, Chunk*> chunks;
};

