#pragma once
#include "common.hpp"
#include "static_vector.hpp"
#include "file.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

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

struct SDL {

	SDL() {
		_sdl = _mix = _img = false;
	}

	void init() {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

		if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
			throw error("SDL_Init failed: %s", SDL_GetError());
		_sdl = true;

		if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) < 0)
			throw error("IMG_Init failed: %s", IMG_GetError());
		_img = true;

		if (Mix_Init(MIX_INIT_OGG) < 0)
			throw error("Mix_Init failed: %s", Mix_GetError());
		_mix = true;
	}

	~SDL() {
		if (_mix) Mix_Quit();
		if (_img) IMG_Quit();
		if (_sdl) SDL_Quit();
	}

	bool _sdl;
	bool _mix;
	bool _img;
};

struct Window {
	void create(const char* title, int w, int h, int winw, int winh) {
		window = SDL_CreateWindow(title,
		                          SDL_WINDOWPOS_UNDEFINED,
		                          SDL_WINDOWPOS_UNDEFINED,
		                          winw,
		                          winh,
		                          SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
		if (window == nullptr) {
			throw error("%s", SDL_GetError());
		}
		context = SDL_GL_CreateContext(window);
		if (context == nullptr) {
			throw error("%s", SDL_GetError());
		}
		SDL_GL_MakeCurrent(window, context);

		glewExperimental = GL_TRUE;
		auto glewrc = glewInit();
		if (glewrc != GLEW_OK) {
			throw error("glew error %s \n", glewGetErrorString(glewrc));
		}
		int major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);
		printf("GL %d.%d\n", major, minor);

		printf("Version: %s\n", glGetString(GL_VERSION));
		printf("Vendor: %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	void present() {
		SDL_GL_SwapWindow(window);
	}

	SDL_Point get_size() {
		SDL_Point sz;
		SDL_GetWindowSize(window, &sz.x, &sz.y);
		return sz;
	}

	SDL_Window* window;
	SDL_GLContext context;
};


struct shader {
	shader(GLenum type) {
		name = glCreateShader(type);
	}

	bool compile(const char* source) {
		glShaderSource(name, 1, &source, nullptr);
		glCompileShader(name);
		GLint status;
		glGetShaderiv(name, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint length;
			glGetShaderiv(name, GL_INFO_LOG_LENGTH, &length);
			std::vector<GLchar> info(length);
			glGetShaderInfoLog(name, length, nullptr, info.data());
			fprintf(stderr, "glCompileShader failed:\n%s\n", info.data());
		}
		return status != GL_FALSE;
	}

	~shader() {
		glDeleteShader(name);
	}

	GLuint name;
};

inline bool compile_program(GLuint program, GLuint shader1, GLuint shader2) {
	glAttachShader(program, shader1);
	glAttachShader(program, shader2);
	glLinkProgram(program);
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		std::vector<GLchar> info(length);
		glGetProgramInfoLog(program, length, nullptr, info.data());
		fprintf(stderr, "glLinkProgram failed: %s\n", info.data());
	}
	glDetachShader(program, shader1);
	glDetachShader(program, shader2);
	return status != GL_FALSE;
}


inline GLuint load_program(const char* vertex_shader, const char* fragment_shader) {
	shader v(GL_VERTEX_SHADER);
	shader f(GL_FRAGMENT_SHADER);
	string vsh = util::read_file(vertex_shader);
	string fsh = util::read_file(fragment_shader);
	v.compile(vsh.c_str());
	f.compile(fsh.c_str());
	GLuint program = glCreateProgram();
	if (!compile_program(program, v.name, f.name))
		throw error("Failed to compile v='%s', f='%s'", vertex_shader, fragment_shader);
	return program;
}

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
struct Texture;

// meshes and materials are handled by the asset manager (only loaded once,
// background loading and swap in etc.)
// that way everything else can just use pointers, pointers are guaranteed
// to be valid by asset manager

// instance: mesh + material + transform
struct Instance {
	Mesh* mesh;
	Material* material;
	std::vector<Texture*> textures;
	mat4 transform;
};

// should the camera just be an instance with a null mesh and material?
struct Camera {
	mat4 transform;
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

