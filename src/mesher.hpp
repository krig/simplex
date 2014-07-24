#pragma once

// API similar to the OpenGL immediate mode,
// outputs a VBO:
// All the vertex data is kept in a single array
// of floats.
class Mesher {
	enum class Use {
		Unused,
		Vertex,
		Color,
		Normal,
		TexCoord,
	};

	struct Format {
		Type type;
		Use use;
		uint8_t length;
	};

	Mesher& init(GLenum mode) {
		_mode = mode;
	}

	// flush and write output to VBO
	void write(VBO& vbo) {
	}

	Mesher& vertex(float x, float y, float z) {
		return *this;
	}

	Mesher& normal(float x, float y, float z) {
		return *this;
	}

	Mesher& color(float r, float g, float b) {
		return *this;
	}

	Mesher& texcoord(float u, float v) {
		return *this;
	}

	GLenum _mode;
	Format _format[32];
	vector<float> _current;
	vector<float> _array;
};

/*
  Mesher mesh;
  vbo = mesh
  .init(GL_TRIANGLES)
  .vertex(0, 0, 0)
  .vertex(0, 0, 0)
  .vertex(0, 0, 0)
  .end();
 */
