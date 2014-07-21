#pragma once

// API similar to the OpenGL immediate mode,
// outputs a VBO
class Mesher {
	enum class Type {
		None,
		Float,
		UnsignedByte,
		SignedByte,
		UnsignedShort,
		SignedShort,
		UnsignedInt,
		SignedInt
	};
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

	Mesh* end() {
		return nullptr;
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
	std::vector<float> _current;
	std::vector<float> _array;
	std::vector<uint32_t> _indices;
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
