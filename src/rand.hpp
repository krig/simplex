#pragma once
#include <random>

namespace util {
	void randseed();
	uint64_t randint();

	typedef uint64_t seed_t;
	typedef std::mt19937_64 random_generator;

}

// distributions
// std::normal_distribution<> d(mean, stddev);
// return d(rng); -> double
// lognormal_distribution<> d(mean, stddev)
// poisson_distribution<> d(mean = 1.0)
// exponential_distribution<> d(lambda = 1.0)


// discrete distribution:
// std::discrete_distribution produces random integers on the interval
// [0, n), where the probability of each individual integer i is
// defined as w i/S, that is the weight of the ith integer divided by
// the sum of all n weights.

// constructor arguments are n weights
// std::mt19937 gen(rd());
// std::discrete_distribution<> d({40, 10, 10, 40});

// std::piecewise_constant_distribution produces random floating-point
// numbers, which are uniformly distributed within each of the several
// subintervals [b i, b i+1), each with its own weight w i. The set of
// interval boundaries and the set of weights are the parameters of
// this distribution.

// 50% of the time, generate a random number between 0 and 1
// 50% of the time, generate a random number between 10 and 15
// std::vector<double> i{0,  1, 10, 15};
// std::vector<double> w{  1,  0,  1};
// std::piecewise_constant_distribution<> d(i.begin(), i.end(), w.begin());

// std::piecewise_linear_distribution produces random floating-point
// numbers, which are distributed according to a linear probability
// density function within each of the several subintervals [b i, b
// i+1). The distribution is such that the probability density at each
// interval boundary is exactly the predefined value p i.

// increase the probability from 0 to 5
// remain flat from 5 to 10
// decrease from 10 to 15 at the same rate
// std::vector<double> i{0, 5, 10, 15};
// std::vector<double> w{0, 1,   1, 0};
// std::piecewise_linear_distribution<> d(i.begin(), i.end(), w.begin());

// so, to generate a resource in a chunk:
// probability for likelihood of occurrence based on y level
// use piecewise_linear_distribution to decide if ore occurs and at
// which heights
// once decided, generate a volume of ore with size random within a
// normal range (use normal_distribution<>) with randomness where the
// center blocks have 100% likelihood of being of the resource and
// distance to center block determines likelihood for other blocks..

// another useful one is std::triangle_distribution
// constructor takes 3 args: min, best_guess, max
// so 0.2, 0.5, 0.8 gives random values between 0.2 and 0.8 with
// highest probability of producing 0.5. It's not a normal
// distribution, the falloff from the most probable value is linear.


