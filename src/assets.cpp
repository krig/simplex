#include "common.hpp"
#include "render.hpp"
#include "assets.hpp"

namespace {
	Material* materials = nullptr;
}

Texture* load_texture(const char* name) {
	SDL_Surface* surface = IMG_Load(name);
	if (!surface)
		throw error("Failed to load %s", name);
	return nullptr;
}

Material* load_material(const char* name) {
	util::strfmt<512> vsh_name("data/%s.vsh", name);
	util::strfmt<512> fsh_name("data/%s.fsh", name);
	Material* nm = new Material;
	nm->program = load_program(vsh_name.c_str(), fsh_name.c_str());
	nm->next = materials;
	materials = nm;
	return nm;
}

void release_assets() {
	while (materials != nullptr) {
		Material* m = materials;
		materials = materials->next;
		delete m;
	}
}
