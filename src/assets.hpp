#pragma once

// mesh -> vertex data
struct Mesh {
	GLuint vao;
	GLuint vbo;
};

struct Texture;

// material -> shaders, texture, animated texture, multitexture
struct Material {
	// ...
	GLuint program;
	Texture* texture;
};

struct Texture {
	// ...
};

Texture* load_texture(const char* name);
Material* load_material(const char* name);

// material <name>
// vertexshader <name>
// fragmentshader <name>
// inputs {
// 3 float vertex
// 3 float normal
// 2 float texcoord
// }

