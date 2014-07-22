#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <memory>
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

using std::string;

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
	inline std::vector<std::string>& operator<<(std::vector<std::string>& v, const char* t) {
		v.push_back(t);
		return v;
	}
}

namespace util {
	inline void bytevector_pad4(std::vector<uint8_t>& vec) {
		const size_t size = vec.size();
		if (size & 3)
			vec.resize((size & ~3) + 4, 0);
	}
}

constexpr double PI = M_PI;
constexpr double TWOPI = PI * 2.0;
constexpr double HALFPI = PI * 0.5;

inline float deg2rad(float deg) {
	return (deg / 360.f) * TWOPI;
}


#include "strfmt.hpp"
#include "logging.hpp"
