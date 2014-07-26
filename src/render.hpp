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


struct VBO {
	VBO() : id(0) {
	}

	~VBO() {
		if (id > 0)
			release();
	}

	bool ok() const {
		return id > 0;
	}

	void gen() {
		glGenBuffers(1, &id);
	}

	void release() {
		glDeleteBuffers(1, &id);
		id = 0;
	}

	void bind(GLenum target) {
		glBindBuffer(target, id);
	}

	void unbind(GLenum target) {
		glBindBuffer(target, 0);
	}

	template <typename V>
	void data(GLenum target, const V& v, GLenum usage = GL_STATIC_DRAW) {
		glBufferData(target, v.size() * sizeof(v[0]), (const void*)v.data(), usage);
	}

	operator GLuint () const {
		return id;
	}

	GLuint id;
};

struct VAO {
	VAO() : id(0) {
	}

	~VAO() {
		if (id > 0)
			release();
	}

	bool ok() const {
		return id > 0;
	}

	void gen() {
		glGenVertexArrays(1, &id);
	}

	void release() {
		glDeleteVertexArrays(1, &id);
		id = 0;
	}

	void bind() {
		glBindVertexArray(id);
	}

	void unbind() {
		glBindVertexArray(0);
	}

	void pointer(GLuint index, GLint size,
	             GLenum type, GLboolean normalized,
	             GLsizei stride, const GLvoid* pointer) {
		glVertexAttribPointer(index, size, type, normalized, stride, pointer);
		glEnableVertexAttribArray(index);
	}

	void attrib(GLuint index, const vec3& v) {
		glVertexAttrib3fv(index, glm::value_ptr(v));
	}

	operator GLuint () const {
		return id;
	}

	GLuint id;
};

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

// instance: mesh + material + frame
struct Instance {
	Frame frame;
	Mesh* mesh;
	Material* material;
	vector<Texture2D*> textures;
};

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

