#pragma once

// the region generator


// generating chunk generates..
// height map
// temperature map
// humidity map
// lowest_height, average_height, maximum_height

template <typename T>
struct ValueRange {
	T minimum;
	T average;
	T maximum;
};

struct ChunkData {
	// this is the first set of data generated for a chunk
	// this is then passed to the biome process which
	// selects a biome based on the information and asks
	// the biome to generate blocks and inputs for population
	// function

	// terrain generation -> biome generation -> structure generation
	// -> population generation
	
	int x, y; // chunk coordinate in chunk space
	ValueRange<int> height;
	ValueRange<float> temperature;
	ValueRange<float> humidity;
	
};

struct Chunk : public ChunkData {

    // height map of chunk, determines to which
	// height rain/snow falls
	uint8_t precipitation_map[256];

	bool is_modified;
	bool has_entities;
	uint64_t last_saved;
	uint64_t inhabited_time;
};
