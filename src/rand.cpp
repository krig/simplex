#include <random>
#include <time.h>
#include "rand.hpp"

namespace {
	std::mt19937_64 rng;
}

namespace util {
	void randseed() {
		rng.seed(time(nullptr));
	}

	uint64_t randint() {
		return rng();
	}
}


