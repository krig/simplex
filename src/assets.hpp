#pragma once

struct Mesh;
struct Material;
struct Texture;
struct Texture2D;

struct Asset {
	virtual ~Asset() {}
	virtual void load() = 0;
	string name;
	Asset* next;
};

void update_assets();

struct Assets {
	Assets();
	~Assets();
};

#include "texture.hpp"
#include "material.hpp"
#include "mesh.hpp"

// material <name>
// vertexshader <name>
// fragmentshader <name>
// inputs {
// 3 float vertex
// 3 float normal
// 2 float texcoord
// }

