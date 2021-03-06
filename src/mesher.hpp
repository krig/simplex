#pragma once

// API similar to the OpenGL immediate mode,
// outputs a VBO:
// All the vertex data is kept in a single array
// of floats.
// todo: error checking
struct Mesher {
	enum Elements {
		Vertex,
		Normal,
		Texcoord,
		Color
	};

	Mesher& begin(GLenum mode, vector<uint8_t> format) {
		_mode = mode;
		_array.clear();
		_format.clear();
		_format.insert(_format.begin(), format.begin(), format.end());
		_vertex = _normal = _texcoord = _color = vec4(0, 0, 0, 0);
		return *this;
	}

	// write output to VBO
	void write(VBO& vbo, GLenum usage = GL_STATIC_DRAW) {
		vbo.data(GL_ARRAY_BUFFER, _array, usage);
	}

	size_t stride() const {
		size_t s = 0;
		for (size_t i = 0; i < _format.size(); i += 2)
			s += _format[i + 1];
		return s;
	}

	size_t nelements() const {
		return _array.size() / stride();
	}

	// set array pointers in VAO
	void set_pointer(size_t i, GLuint index) {
		size_t estride = stride();
		size_t offset = 0;
		for (size_t n = 0; n < i*2; n += 2)
			offset += _format[n + 1] * sizeof(float);

		glVertexAttribPointer(index, _format[i*2 + 1], GL_FLOAT, GL_FALSE, estride*sizeof(float), (const void*)offset);
		glEnableVertexAttribArray(i);
	}

	void draw() {
		glDrawArrays(_mode, 0, nelements());
	}

	Mesher& vertex(const vec2& v) { _vertex.x = v.x; _vertex.y = v.y; _push(); return *this; }
	Mesher& vertex(const vec3& v) { _vertex.x = v.x; _vertex.y = v.y; _vertex.z = v.z; _push(); return *this; }
	Mesher& vertex(const vec4& v) { _vertex.x = v.x; _vertex.y = v.y; _vertex.z = v.z; _vertex.w = v.w; _push(); return *this; }

	Mesher& normal(const vec2& v) { _normal.x = v.x; _normal.y = v.y; return *this; }
	Mesher& normal(const vec3& v) { _normal.x = v.x; _normal.y = v.y; _normal.z = v.z; return *this; }
	Mesher& normal(const vec4& v) { _normal.x = v.x; _normal.y = v.y; _normal.z = v.z; _normal.w = v.w; return *this; }

	Mesher& texcoord(const vec2& v) { _texcoord.x = v.x; _texcoord.y = v.y; return *this; }
	Mesher& texcoord(const vec3& v) { _texcoord.x = v.x; _texcoord.y = v.y; _texcoord.z = v.z; return *this; }
	Mesher& texcoord(const vec4& v) { _texcoord.x = v.x; _texcoord.y = v.y; _texcoord.z = v.z; _texcoord.w = v.w; return *this; }

	Mesher& color(const vec2& v) { _color.x = v.x; _color.y = v.y; return *this; }
	Mesher& color(const vec3& v) { _color.x = v.x; _color.y = v.y; _color.z = v.z; return *this; }
	Mesher& color(const vec4& v) { _color.x = v.x; _color.y = v.y; _color.z = v.z; _color.w = v.w; return *this; }

	void _push() {
		for (size_t i = 0; i < _format.size(); i += 2) {
			switch (_format[i]) {
			case Vertex: _array.insert(_array.end(), glm::value_ptr(_vertex), glm::value_ptr(_vertex) + _format[i + 1]); break;
			case Color: _array.insert(_array.end(), glm::value_ptr(_color), glm::value_ptr(_color) + _format[i + 1]); break;
			case Normal: _array.insert(_array.end(), glm::value_ptr(_normal), glm::value_ptr(_normal) + _format[i + 1]); break;
			case Texcoord: _array.insert(_array.end(), glm::value_ptr(_texcoord), glm::value_ptr(_texcoord) + _format[i + 1]); break;
			}
		}
	}

	GLenum _mode;
	vec4 _vertex;
	vec4 _normal;
	vec4 _texcoord;
	vec4 _color;
	vector<uint8_t> _format;
	vector<float> _array;
};

/*
  Mesher mesh;
  vbo = mesh
  .begin(GL_TRIANGLES, {Mesher::Color, 3, Mesher::Vertex, 3})
  .color(0, 0, 0)
  .vertex(0, 0, 0)
  .vertex(0, 0, 0)
  .vertex(0, 0, 0)
  .end();

  Mesh* geo::make_cube();
  make_instance(geo::make_cube(), fancy_material);
  
 */
