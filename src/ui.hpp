#pragma once
#include "mesher.hpp"

struct UiStack {

	void init() {
		_vbo.gen();
		_material = load_material("ui");
		_8x8 = load_texture("data/8x8font.png");
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
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		_material->use();
		_8x8->bind(0);
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
		const float scale = 16.f;
		float max_x = 0.f;
		float cur_x = 0.f;
		float y_sum = 0.f;
		for (const char* s = str; *s; ++s) {
			if (*s == '\n') {
				if (cur_x > max_x)
					max_x = cur_x;
				cur_x = 0.f;
				y_sum += scale;
			} else {
				cur_x += scale;
				if (cur_x > max_x)
					max_x = cur_x;
			}
		}
		return vec2(max_x, y_sum);
	}

	// pos is top left corner of text
	void text(vec2 pos, Color clr, const char* str, ...) {
			_vertices.begin(GL_TRIANGLES, { Mesher::Vertex, 2, Mesher::Texcoord, 2, Mesher::Color, 4 });
			_vertices.color(clr);

			char buf[256];
			va_list va_args;
			va_start(va_args, str);
			vsnprintf(buf, 256, str, va_args);
			va_end(va_args);
			size_t len = strlen(buf);

			const float scale = 16.f;
			const float d = 8.f / 1024.f;

			vec2 rpos(pos);
			for (size_t i = 0; i < len; ++i) {
				if (buf[i] < 0 || buf[i] > 127)
					continue;
				if (buf[i] == '\n') {
					rpos.x = pos.x;
					rpos.y += scale;
					continue;
				}
				vec2 tc(buf[i] * d, 0.f);
				vec2 p1(rpos.x, rpos.y);
				vec2 p2(rpos.x + scale, rpos.y + scale);
				_quad(p1, p2, vec2(tc.x, 0.f), vec2(tc.x + d, 1.f));
				rpos.x += scale;
			}

			_vertices.write(_vbo, GL_STREAM_DRAW);
			_vertices.set_pointer(0, _position);
			_vertices.set_pointer(1, _texcoord);
			_vertices.set_pointer(2, _color);
			_vertices.draw();

	}

	void line(vec2 p1, vec2 p2, Color clr, float thickness = 1.f) {
		const float d = 8.f / 1024.f;
		_vertices.begin(GL_LINES, { Mesher::Vertex, 2, Mesher::Texcoord, 2, Mesher::Color, 4 });
		_vertices.color(clr);
		_vertices.texcoord(vec2(0, 0));
		_vertices.vertex(p1);
		_vertices.texcoord(vec2(d, d));
		_vertices.vertex(p2);
		_vertices.write(_vbo, GL_STREAM_DRAW);
		_vertices.set_pointer(0, _position);
		_vertices.set_pointer(1, _texcoord);
		_vertices.set_pointer(2, _color);
		_vertices.draw();
	}

	void rect(vec2 p1, vec2 p2, Color clr, bool fill) {
		const float d = 8.f / 1024.f;
		if (fill) {
			_vertices.begin(GL_TRIANGLES, { Mesher::Vertex, 2, Mesher::Texcoord, 2, Mesher::Color, 4 });
			_vertices.color(clr);
			_quad(p1, p2, vec2(0, 0), vec2(d, d));
			_vertices.write(_vbo, GL_STREAM_DRAW);
			_vertices.set_pointer(0, _position);
			_vertices.set_pointer(1, _texcoord);
			_vertices.set_pointer(2, _color);
			_vertices.draw();
		} else {
			_vertices.begin(GL_LINE_LOOP, { Mesher::Vertex, 2, Mesher::Texcoord, 2, Mesher::Color, 4 });
			_vertices.color(clr);
			_vertices.texcoord(vec2(0, 0));
			_vertices.vertex(p1);
			_vertices.texcoord(vec2(d, d));
			_vertices.vertex(vec2(p1.x, p2.y));
			_vertices.texcoord(vec2(0, 0));
			_vertices.vertex(p2);
			_vertices.texcoord(vec2(d, d));
			_vertices.vertex(vec2(p2.x, p1.y));
			_vertices.write(_vbo, GL_STREAM_DRAW);
			_vertices.set_pointer(0, _position);
			_vertices.set_pointer(1, _texcoord);
			_vertices.set_pointer(2, _color);
			_vertices.draw();
		}
	}

	void _quad(vec2 p1, vec2 p2, vec2 t1, vec2 t2) {
		_vertices.texcoord(vec2(t1.x, t2.y)).vertex(p1);
		_vertices.texcoord(vec2(t2.x, t1.y)).vertex(p2);
		_vertices.texcoord(vec2(t1.x, t1.y)).vertex(vec2(p1.x, p2.y));
		_vertices.texcoord(vec2(t2.x, t1.y)).vertex(p2);
		_vertices.texcoord(vec2(t1.x, t2.y)).vertex(p1);
		_vertices.texcoord(vec2(t2.x, t2.y)).vertex(vec2(p2.x, p1.y));
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
	Texture2D* _8x8;
	Mesher _vertices;
	GLuint _position, _texcoord, _color;
};
