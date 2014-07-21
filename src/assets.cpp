#include "common.hpp"
#include "render.hpp"
#include "assets.hpp"
#include "notify.hpp"

namespace {
	Notify notify;
	Asset* assets = nullptr;
	std::map<int, Asset*> wdmap;

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
