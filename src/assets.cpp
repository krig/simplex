#include "common.hpp"
#include "render.hpp"
#include "assets.hpp"
#include "notify.hpp"

namespace {
	Notify notify;
	Asset* assets = nullptr;
	dict<int, Asset*> wdmap;

	GLenum sdl_format_to_gl(SDL_Surface* image) {
		GLenum format;
		switch (image->format->BytesPerPixel) {
		case 4:
			format = (image->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;
			break;
		case 3:
			format = (image->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR;
			break;
		default:
			return 0;
		}
		return format;
	}

	void on_asset_change(inotify_event* event) {
		auto e = wdmap.find(event->wd);
		if (e != wdmap.end()) {
			LOG_INFO("Changed: %s", e->second->name.c_str());
			e->second->load();
		}
	}
}

Assets::Assets() {
}

Assets::~Assets() {
	while (assets != nullptr) {
		Asset* a = assets;
		assets = a->next;
		delete a;
	}
}

void update_assets() {
	notify.update(on_asset_change);
}

Texture2D::Texture2D() {
	id = 0;
	w = h = 0;
}

Texture2D::~Texture2D() {
	glDeleteTextures(1, &id);
}

void Texture2D::bind(int i) {
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture2D::load() {
	SDL_Surface* image = IMG_Load(name.c_str());
	if (!image)
		throw error("Failed to load %s", name.c_str());

	SDL_SetSurfaceAlphaMod(image, 0xFF);
	SDL_SetSurfaceBlendMode(image, SDL_BLENDMODE_NONE);

	if (id != 0)
		glDeleteTextures(1, &id);

	glGenTextures( 1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    w = image->w;
    h = image->h;
    GLenum format = sdl_format_to_gl(image);
    if (format == 0)
	    throw error("Unsupported texture format: %s", name.c_str());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->w, image->h,
                 0, format, GL_UNSIGNED_BYTE, image->pixels);

	SDL_FreeSurface(image);

}

Texture2D* load_texture(const char* name) {
	Texture2D* t = new Texture2D;
	t->name = name;
	t->load();
	t->next = assets;
	assets = t;
	wdmap[notify.add(name)] = t;
	return t;
}

namespace {
struct Shader {
	Shader(GLenum type) {
		name = glCreateShader(type);
	}

	~Shader() {
		glDeleteShader(name);
	}

	bool compile(const char* source) {
		glShaderSource(name, 1, &source, nullptr);
		glCompileShader(name);
		GLint status;
		glGetShaderiv(name, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint length;
			glGetShaderiv(name, GL_INFO_LOG_LENGTH, &length);
			vector<GLchar> info(length);
			glGetShaderInfoLog(name, length, nullptr, info.data());
			LOG_ERROR("glCompileShader failed: %s", info.data());
		}
		return status != GL_FALSE;
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
		vector<GLchar> info(length);
		glGetProgramInfoLog(program, length, nullptr, info.data());
		LOG_ERROR("glLinkProgram failed: %s", info.data());
	}
	glDetachShader(program, shader1);
	glDetachShader(program, shader2);
	return status != GL_FALSE;
}


inline GLuint load_program(const char* vertex_shader, const char* fragment_shader) {
	Shader v(GL_VERTEX_SHADER);
	Shader f(GL_FRAGMENT_SHADER);
	string vsh = util::read_file(vertex_shader);
	string fsh = util::read_file(fragment_shader);
	if (!v.compile(vsh.c_str()))
		return 0;
	if (!f.compile(fsh.c_str()))
		return 0;
	GLuint program = glCreateProgram();
	if (!compile_program(program, v.name, f.name)) {
		LOG_ERROR("Failed to compile v='%s', f='%s'", vertex_shader, fragment_shader);
		return 0;
	}
	return program;
}

}

void Material::load() {
	util::strfmt<512> vsh_name("data/%s.vsh", name.c_str());
	util::strfmt<512> fsh_name("data/%s.fsh", name.c_str());
	GLuint newprogram = load_program(vsh_name.c_str(), fsh_name.c_str());
	if (newprogram == 0)
		throw error("Failed to load program %s", name.c_str());
	if (program != 0)
		glDeleteProgram(program);
	program = newprogram;
}

Material* load_material(const char* name) {
	Material* nm = new Material;
	nm->program = 0;
	nm->name = name;
	nm->load();
	nm->next = assets;
	assets = nm;
	util::strfmt<512> vsh_name("data/%s.vsh", name);
	util::strfmt<512> fsh_name("data/%s.fsh", name);
	wdmap[notify.add(vsh_name.c_str())] = nm;
	wdmap[notify.add(fsh_name.c_str())] = nm;
	return nm;
}
