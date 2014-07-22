#pragma once

#include "assets.hpp"

struct Texture;
struct Texture2D;

struct Texture : public Asset {
};

struct Texture2D : public Texture {
	Texture2D();
	~Texture2D();

	void load();
	void bind(int i);

	GLuint id;
	int w, h;
};

Texture2D* load_texture(const char* name);
