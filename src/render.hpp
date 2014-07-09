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

struct renderer {
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

struct shader_program {
	shader_program() {
		name = glCreateProgram();
	}

	bool compile(GLuint shader1, GLuint shader2) {
		glAttachShader(name, shader1);
		glAttachShader(name, shader2);
		glLinkProgram(name);
		GLint status;
		glGetProgramiv(name, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			GLint length;
			glGetProgramiv(name, GL_INFO_LOG_LENGTH, &length);
			std::vector<GLchar> info(length);
			glGetProgramInfoLog(name, length, nullptr, info.data());
			fprintf(stderr, "glLinkProgram failed: %s\n", info.data());
		}
		glDetachShader(name, shader1);
		glDetachShader(name, shader2);
		return status != GL_FALSE;
	}

	~shader_program() {
		glDeleteProgram(name);
	}

	void use() {
		glUseProgram(name);
	}

	void uniform(const char* loc, const mat4& m) {
		GLuint index = glGetUniformLocation(name, loc);
		glUniformMatrix4fv(index, 1, GL_FALSE, glm::value_ptr(m));
	}

	void vertex_attrib(const char* loc, const vec3& v) {
		GLuint i = glGetAttribLocation(name, loc);
		glVertexAttrib3f(i, v.x, v.y, v.z);
	}

	void vertex_attrib(const char* loc, const float* p, GLuint num, GLuint stride) {
		GLuint i = glGetAttribLocation(name, loc);
		glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
		glEnableVertexAttribArray(i);
	}

	GLuint name;
};

inline std::unique_ptr<shader_program> load_program(const char* vertex_shader, const char* fragment_shader) {
	shader v(GL_VERTEX_SHADER);
	shader f(GL_FRAGMENT_SHADER);
	string vsh = util::read_file(vertex_shader);
	string fsh = util::read_file(fragment_shader);
	v.compile(vsh.c_str());
	f.compile(fsh.c_str());
	std::unique_ptr<shader_program> program(new shader_program);
	program->compile(v.name, f.name);
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
void fill_static_vbo(GLuint vbo, const V& v) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(v[0]), (const void*)v.data(), GL_STATIC_DRAW);
}

struct renderobj {
	shader_program* material;
	mat4 transform;

	std::vector<GLuint> buffers;
	std::vector<GLuint> arrays;
};


