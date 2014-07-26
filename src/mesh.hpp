#pragma once

struct Mesh;

// mesh -> vertex data
// indexed element buffer
struct Mesh : public Asset {
	void load() {
		// no asset reloading
	}

	VAO object;
	VBO vertices;
	VBO indices;

	GLenum mode;
	GLenum index_type;
	GLuint ntriangles;
	GLuint nindices;

	void draw_instanced(uint32_t ninstances) {
		object.bind();
		if (indices.ok()) {
			glDrawElementsInstanced(mode, nindices, index_type, 0, ninstances);
		} else {
			glDrawArraysInstanced(mode, 0, ntriangles, ninstances);
		}
		object.unbind();
	}

	void draw() {
		object.bind();
		if (indices.ok()) {
			glDrawElements(mode, nindices, index_type, 0);
		} else {
			glDrawArrays(mode, 0, ntriangles);
		}
		object.unbind();
	}

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

// Mesh* create_mesh(const char* name, Mesher* source);
