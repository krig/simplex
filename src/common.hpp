#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
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

#include "strfmt.hpp"
#include "logging.hpp"

#include "sdl2.hpp"
