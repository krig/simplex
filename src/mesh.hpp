#pragma once

struct Mesh;

// mesh -> vertex data
// indexed element buffer
struct Mesh {
	GLuint vao;

	GLenum mode;
	GLuint ntriangles;
	GLuint nindices;

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

