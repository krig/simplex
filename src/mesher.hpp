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

	struct Array {
		Format _format[32];
		std::vector<uint8_t> data;
	};


	void set_mode(GLenum mode);
	Mesher& begin(Format e0);
	void end();

	Mesher& vertex(float x, float y, float z);

	Mesher& color(float r, float g, float b);

	GLenum mode;
	Array* _current;
	std::vector<Array> _arrays;
};


vb.begin({Mesher::Use::Vertex, Mesher::Type::Float, 3});
vb.end();

vb.begin({Mesher::Use::Normal, Mesher::Type::Float, 2});
vb.end();
