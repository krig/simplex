#pragma once

void worley_noise_impl(const glm::dvec3 &p, double density, int num,
                       double *dist, glm::dvec3 *vec);

/* Return the distance from the point p to the nth closest randomly
   distributed point defined by the noise function.  Density sets how
   densely the noise points are in three-space. */
inline void worley_noise(const glm::dvec3 &p, double* d, size_t N, double density) {
	worley_noise_impl(p, density, N, d, 0);
	for (size_t i = 0; i < N; ++i)
		d[i] = sqrt(d[i]);
}

/* Similarly, return the vector the nth closest noise point. */
template <size_t N>
glm::dvec3 worley_noise_vector(const glm::dvec3 &p, int nth, double density) {
	glm::dvec3 v[N];
	worley_noise_impl(p, density, nth, 0, v);
	return v[nth - 1];
}
