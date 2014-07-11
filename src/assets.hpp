#pragma once

struct Mesh;
struct Material;
struct Texture;


// mesh -> vertex data
struct Mesh {
	GLuint vao;
	GLuint vbo;

	GLuint narrays;
	GLuint ntriangles;
};

struct Asset {
	virtual ~Asset() {}
	Asset* next;
};



// material -> shaders, texture, animated texture, multitexture
struct Material : public Asset {
	// ...

	~Material() {
		glDeleteProgram(program);
	}

	void use() {
		glUseProgram(program);
	}

	void uniform(const char* loc, const vec3& v) {
		GLuint index = glGetUniformLocation(program, loc);
		glUniform3fv(index, 1, glm::value_ptr(v));
	}

	void uniform(const char* loc, const mat4& m) {
		GLuint index = glGetUniformLocation(program, loc);
		glUniformMatrix4fv(index, 1, GL_FALSE, glm::value_ptr(m));
	}

	void uniform(const char* loc, int i) {
		GLuint index = glGetUniformLocation(program, loc);
		glUniform1i(index, i);
	}

	void vertex_attrib(const char* loc, const vec3& v) {
		GLuint i = glGetAttribLocation(program, loc);
		glVertexAttrib3f(i, v.x, v.y, v.z);
	}

	void vertex_attrib(const char* loc, const float* p, GLuint num, GLuint stride) {
		GLuint i = glGetAttribLocation(program, loc);
		glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
		glEnableVertexAttribArray(i);
	}

	GLuint program;
};

struct Texture : public Asset {

	~Texture() {
		glDeleteTextures(1, &texture);
	}

	void bind(int i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	GLuint texture;
	int w, h;
};

Texture* load_texture(const char* name);
Material* load_material(const char* name);
void release_assets();

// material <name>
// vertexshader <name>
// fragmentshader <name>
// inputs {
// 3 float vertex
// 3 float normal
// 2 float texcoord
// }

