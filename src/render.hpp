#pragma once
#include "common.hpp"
#include "static_vector.hpp"
#include "file.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using glm::quat;

struct SDL {

	SDL();
	~SDL();
	void init();

	bool _sdl;
	bool _mix;
	bool _img;
};

struct Window {
	void create(const char* title, int w, int h, int winw, int winh);
	void present();
	SDL_Point get_size() const;

	SDL_Window* window;
	SDL_GLContext context;
};


inline GLuint make_vao() {
	GLuint id;
	glGenVertexArrays(1, &id);
	return id;
}

inline void bind_vao(GLuint vao) {
	glBindVertexArray(vao);
}

inline GLuint make_vbo() {
	GLuint id;
	glGenBuffers(1, &id);
	return id;
}

template <class V>
GLuint make_buffer(GLenum target, const V& v, GLenum usage = GL_STATIC_DRAW) {
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(target, vbo);
	glBufferData(target, v.size() * sizeof(v[0]), (const void*)v.data(), usage);
	return vbo;
}

// defined in asset manager
struct Mesh;
struct Material;
struct Texture2D;

// meshes and materials are handled by the asset manager (only loaded once,
// background loading and swap in etc.)
// that way everything else can just use pointers, pointers are guaranteed
// to be valid by asset manager

struct ChunkPos {
	int32_t x;
	int32_t z;

	inline vec3 update(vec3 offset) {
		const double fx = floor(offset.x), fz = floor(offset.z);
		x += fx;
		z += fz;
		offset.x -= fx;
		offset.z -= fz;
		return offset;
	}
};

// position in chunk coordinates
// allows for high precision even
// at crazy coordinates
struct Frame {
	ChunkPos chunk;
	vec3 offset; // x and z always in [0, 1)
	quat rot;

	void translate(vec3 dv) {
		// translate maintaining chunk/offset invariant
		offset = chunk.update(offset + dv);
	}

	// position relative to root chunk
	vec3 position(ChunkPos root) const {
		return vec3((chunk.x - root.x) + offset.x,
		            offset.y,
		            (chunk.z - root.z) + offset.z);
	}

	// transform matrix relative to root chunk
	mat4 transform(ChunkPos root) const {
		return glm::translate(mat4(orientation()), position(root));
	}

	// orientation
	mat3 orientation() const {
		return glm::toMat3(rot);
	}
};

// instance: mesh + material + transform
struct Instance {
	Frame frame;
	Mesh* mesh;
	Material* material;
	std::vector<Texture2D*> textures;
};


// should the camera just be an instance with a null mesh and material?
struct Camera {
	Frame frame;

	// calculates viewmodel matrix relative
	// to the root chunk
	mat4 viewmodel(ChunkPos root) const {
		return glm::translate(mat4(glm::transpose(frame.orientation())),
		                      -frame.position(root));
	}
};

struct Renderable {
	Material* material;
	Mesh* mesh;
	std::array<Texture2D*, 16> textures;
};

//struct Instance {
//	Renderable* renderable;
//	SceneLocation location;
//};

// * in game, keep separate arrays for instances of different type
// * sort arrays by shader first and texture second before rendering

// in draw...
// draw_sky()
// draw_terrain()
// draw_trees()
// draw_plants()
// draw_items()
// draw_creatures()
// draw_players()

