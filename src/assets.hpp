#pragma once

struct Mesh;
struct Material;
struct Texture2D;


// mesh -> vertex data
struct Mesh {
	GLuint vao;
	GLuint vbo;

	GLenum mode;
	GLuint narrays;
	GLuint ntriangles;

	// glDrawArrays(mode, first, count)
	// glDrawArraysInstanced(mode, first, count, primcount)
	// glDrawElements(mode, count, type, indices)
	// glDrawElementsInstanced(mode, count, type, indices, primcount)
	// glDrawRangeElements(mode, start, end, count, type, indices)
	// gl_VertexID

	// generate full-screen triangle in vertex shader
	// bind empty VAO
	// call glDrawArrays(GL_TRIANGLES, 0, 3);
	// in vertex shader:
	// vertex shader
	// out vec2 vTexCoord;

	// void main()
	// {
	// vTexCoord   = vec2( (gl_VertexID << 1) & 2, gl_VertexID & 2 );
	// gl_Position = vec4( vTexCoord * 2.0 - 1.0, 0.0, 1.0 );
	// }
};

struct Asset {
	virtual ~Asset() {}
	virtual void load() = 0;
	string name;
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

	void load();

	GLuint program;
};

struct Texture2D : public Asset {
	Texture2D();
	~Texture2D();

	void load();
	void bind(int i);

	GLuint id;
	int w, h;
};

void update_assets();
Texture2D* load_texture(const char* name);
Material* load_material(const char* name);

struct Assets {
	Assets();
	~Assets();
};

// material <name>
// vertexshader <name>
// fragmentshader <name>
// inputs {
// 3 float vertex
// 3 float normal
// 2 float texcoord
// }

