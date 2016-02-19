#include "common.hpp"
#include "render.hpp"
#include "assets.hpp"
#include "notify.hpp"

#include <fstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

namespace {
	Notify notify;
	dict<string, Asset*> assets;
	dict<int, Asset*> wdmap;

	static inline std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	GLenum sdl_format_to_gl(SDL_Surface* image) {
		GLenum format;
		switch (image->format->BytesPerPixel) {
		case 4:
			format = (image->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;
			break;
		case 3:
			format = (image->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR;
			break;
		case 1:
			format = GL_RED;
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

	void add_notification(const char* file, Asset* asset) {
		wdmap[notify.add(file)] = asset;
	}

	void init_materials() {
		string line;
		std::ifstream infile("data/materials.conf");
		while (std::getline(infile, line)) {
			rtrim(line);
			unique_ptr<Material> nm(new Material);
			nm->program = 0;
			nm->name = line;
			string v = line + ".vsh";
			string f = line + ".fsh";
			nm->vshader = "data/";
			nm->vshader += v;
			nm->fshader = "data/";
			nm->fshader += f;
			nm->load();
			add_notification(nm->vshader.c_str(), nm.get());
			add_notification(nm->fshader.c_str(), nm.get());
			assets[nm->name] = nm.release();
		}
	}
}

Assets::Assets() {
	init_materials();
}

Assets::~Assets() {
	for (auto& e : assets)
		delete e.second;
	assets.clear();
	wdmap.clear();
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
    if (format == GL_RED) {
	    LOG_INFO("monochrome: %dx%d", w, h);
	    vector<SDL_Color> rgba(w * h);
	    const uint8_t* px = (const uint8_t*)image->pixels;
	    for (int i = 0; i < w * h; ++i) {
		    rgba[i] = image->format->palette->colors[px[i]];
	    }
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->w, image->h,
	                 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    }
    else {
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->w, image->h,
	                 0, format, GL_UNSIGNED_BYTE, image->pixels);
    }

	SDL_FreeSurface(image);

}

Texture2D* load_texture(const char* name) {
	auto ti = assets.find(name);
	if (ti != assets.end())
		return (Texture2D*)(ti->second);
	Texture2D* t = new Texture2D;
	t->name = name;
	t->load();
	assets[name] = t;
	add_notification(name, t);
	return t;
}

Texture2D* gen_texture2(const char* name, int w, int h, ColorFiller* filler) {
	auto ti = assets.find(name);
	Texture2D* t = 0;
	if (ti == assets.end()) {
		t = new Texture2D;
		t->name = name;
		glGenTextures( 1, &t->id);
	} else {
		t = (Texture2D*)(ti->second);
	}

	glBindTexture(GL_TEXTURE_2D, t->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    t->w = w;
    t->h = h;
    vector<uint32_t> pixels(w*h, 0xffffffff);
    for (int iy = 0; iy < h; ++iy)
	    for (int ix = 0; ix < w; ++ix)
		    pixels[iy*w + ix] = (*filler)(ix, iy).rgba8();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
	assets[name] = t;
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
	GLuint newprogram = load_program(vshader.c_str(), fshader.c_str());
	if (newprogram == 0)
		throw error("Failed to load program %s", name.c_str());
	if (program != 0)
		glDeleteProgram(program);
	program = newprogram;
}

Material* load_material(const char* name) {
	auto mi = assets.find(name);
	if (mi != assets.end())
		return (Material*)(mi->second);
	throw error("Material not found: %s", name);
}
