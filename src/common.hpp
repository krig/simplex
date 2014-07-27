#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <utility>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "GL/glew.h"
#include <SDL_opengl.h>
#include "glm/glm.hpp"
#define GLM_FORCE_RADIANS
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/noise.hpp"

// pull some commonly used stuff into our namespace
using std::string;
using std::move;
using std::swap;
using std::make_pair;
template <typename T> using vector = std::vector<T>;
template <typename T, size_t N> using array = std::array<T, N>;
template <typename P> using unique_ptr = std::unique_ptr<P>;
template <typename F, typename S> using pair = std::pair<F, S>;
template <typename K, typename V> using dict = std::unordered_map<K, V>;
template <typename V> using set = std::unordered_set<V>;


#define ASIZE(a) (sizeof(a)/sizeof((a)[0]))

struct error : public std::exception {
	error(const char* fmt, ...) {
		va_list va_args;
		va_start(va_args, fmt);
		vsnprintf(_buf, 2048, fmt, va_args);
		va_end(va_args);
	}

	virtual const char* what() const noexcept {
		return _buf;
	}

private:
	char _buf[2048];
};

namespace std {
	inline vector<string>& operator<<(vector<string>& v, const char* t) {
		v.push_back(t);
		return v;
	}
}

namespace util {
	template <size_t Pad = 4>
	inline void bytevector_pad(vector<uint8_t>& vec) {
		const size_t size = vec.size();
		if (size & (Pad - 1))
			vec.resize((size & ~(Pad - 1)) + Pad, 0);
	}
}

constexpr double PI = M_PI;
constexpr double TWOPI = PI * 2.0;
constexpr double HALFPI = PI * 0.5;
constexpr double TAU = 1.618033988749895; // golden ratio

constexpr float deg2rad(float deg) {
	return (deg / 360.f) * TWOPI;
}

template <typename I>
inline void delete_all(I begin, I end) {
	for (I i = begin; i != end; ++i)
		delete *i;
}

#include "strfmt.hpp"
#include "logging.hpp"
