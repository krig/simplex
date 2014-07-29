#pragma once
#include "mesher.hpp"

struct UiStack {

	void init() {
		_vbo.gen();
		_material = load_material("ui");
		_white = gen_texture("*ui-blank*", 16, 16, [](int x, int y) {
				return Color(0xffffffff); });
		_position = _material->attrib("position");
		_texcoord = _material->attrib("in_texcoord");
		_color = _material->attrib("in_color");
	}

	void begin(SDL_Point screensize) {
		_screensize.x = screensize.x;
		_screensize.y = screensize.y;

		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		_material->use();
		_white->bind(0);
		_material->uniform("tex0", 0);
		_material->uniform("screensize", _screensize);
		_vbo.bind(GL_ARRAY_BUFFER);
	}

	void end() {
		// set projection
		// set gl state
		// disable z buffer

		_vbo.unbind();

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
	}

	vec2 measure_text(const char* str, ...) {
		return vec2(0, 0);
	}

	// pos is top left corner of text
	void text(vec2 pos, Color clr, const char* str, ...) {
	}

	void line(vec2 p1, vec2 p2, Color clr, float thickness = 1.f) {
		_vertices.begin(GL_LINES, { Mesher::Vertex, 2, Mesher::Texcoord, 2, Mesher::Color, 4 });
		_vertices.color(clr);
		_vertices.texcoord(vec2(0, 0));
		_vertices.vertex(p1);
		_vertices.color(clr);
		_vertices.texcoord(vec2(1, 1));
		_vertices.vertex(p2);
		_vertices.write(_vbo, GL_STREAM_DRAW);
		_vertices.set_pointer(0, _position);
		_vertices.set_pointer(1, _texcoord);
		_vertices.set_pointer(2, _color);
		_vertices.draw();
	}

	void rect(vec2 p1, vec2 p2, Color clr, bool fill) {
		if (fill) {
			_vertices.begin(GL_TRIANGLES, { Mesher::Vertex, 2, Mesher::Texcoord, 2, Mesher::Color, 4 });
			_vertices.color(clr);
			_vertices.texcoord(vec2(0, 0));
			_vertices.vertex(p1);
			_vertices.texcoord(vec2(1.f, 1.f));
			_vertices.vertex(p2);
			_vertices.texcoord(vec2(0.f, 1.f));
			_vertices.vertex(vec2(p1.x, p2.y));
			_vertices.texcoord(vec2(1.f, 1.f));
			_vertices.vertex(p2);
			_vertices.texcoord(vec2(0, 0));
			_vertices.vertex(p1);
			_vertices.texcoord(vec2(1.f, 0.f));
			_vertices.vertex(vec2(p2.x, p1.y));
			_vertices.write(_vbo, GL_STREAM_DRAW);
			_vertices.set_pointer(0, _position);
			_vertices.set_pointer(1, _texcoord);
			_vertices.set_pointer(2, _color);
			_vertices.draw();
		}
	}

	void circle(vec2 pos, float r, Color clr, bool fill) {
	}

	void button() {
	}

	// 0 = horizontal
	// 1 = vertical
	void slider(int direction) {
	}

	vec2 _screensize;
	VBO _vbo;
	Material* _material;
	Texture2D* _white;
	Mesher _vertices;
	GLuint _position, _texcoord, _color;
};
