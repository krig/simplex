#pragma once

#include "assets.hpp"
#include "colors.hpp"

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

struct ColorFiller {
	virtual Color operator()(int x, int y) = 0;
};

template <typename Fn>
struct FillwrapperImpl : public ColorFiller {
	FillwrapperImpl(Fn fn) : fn(fn) {}
	virtual Color operator()(int x, int y) {
		return fn(x, y);
	}
	Fn fn;
};

Texture2D* gen_texture2(const char* name, int w, int h, ColorFiller* fillfn);

template <typename Fn>
Texture2D* gen_texture(const char* name, int w, int h, Fn fillfn) {
	FillwrapperImpl<Fn> f(fillfn);
	return gen_texture2(name, w, h, &f);
}
