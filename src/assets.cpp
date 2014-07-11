#include "common.hpp"
#include "render.hpp"
#include "assets.hpp"

namespace {
	Asset* assets = nullptr;

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
}

Texture* load_texture(const char* name) {
	SDL_Surface* image = IMG_Load(name);
	if (!image)
		throw error("Failed to load %s", name);

	SDL_SetSurfaceAlphaMod(image, 0xFF);
	SDL_SetSurfaceBlendMode(image, SDL_BLENDMODE_NONE);

	Texture* t = new Texture;

	glGenTextures( 1, &t->texture);
	glBindTexture(GL_TEXTURE_2D, t->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    t->w = image->w;
    t->h = image->h;
    GLenum format = sdl_format_to_gl(image);
    if (format == 0)
	    throw error("Unsupported texture format: %s", name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->w, image->h,
                 0, format, GL_UNSIGNED_BYTE, image->pixels);

	SDL_FreeSurface(image);
	t->next = assets;
	assets = t;
	return t;
}

Material* load_material(const char* name) {
	util::strfmt<512> vsh_name("data/%s.vsh", name);
	util::strfmt<512> fsh_name("data/%s.fsh", name);
	Material* nm = new Material;
	nm->program = load_program(vsh_name.c_str(), fsh_name.c_str());
	nm->next = assets;
	assets = nm;
	return nm;
}

void release_assets() {
	while (assets != nullptr) {
		Asset* a = assets;
		assets = a->next;
		delete a;
	}
}
