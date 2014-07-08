#pragma once
#include "common.hpp"
#include "static_vector.hpp"

namespace sdl {

	struct error : public std::exception {
		error(const char* fmt, ...) {
			va_list va_args;
			va_start(va_args, fmt);
			vsnprintf(_buf, 2048, fmt, va_args);
			va_end(va_args);
		}

		virtual const char* what() const noexcept {
			return _buf;
		}

	private:
		char _buf[2048];
	};

	inline void delay_to_fps(const Uint32 delta,
	                         const Uint32 min_ticks_per_frame = (Uint32)((1.0 / 60.0) * 1000.0)) {
		if (delta < min_ticks_per_frame) {
			SDL_Delay(min_ticks_per_frame - delta);
		}
		else {
			LOG_TRACE("%u / %u", delta, min_ticks_per_frame);
		}
	}


	struct core {
		core() {
			if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
				throw error("SDL_Init failed: %s", SDL_GetError());
		}

		~core() {
			SDL_Quit();
		}
	};

	//struct ttf {
	//	ttf() {
	//		if (TTF_Init() < 0) {
	//			throw error("TTF_Init failed: %s", TTF_GetError());
	//		}
	//	}
	//
	// ~ttf() {
	//		TTF_Quit();
	//	}
	//};

	struct image {
		image() {
			if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) < 0) {
				throw error("IMG_Init failed: %s", IMG_GetError());
			}
		}

		~image() {
			IMG_Quit();
		}
	};

	struct mixer {
		mixer() {
			if (Mix_Init(MIX_INIT_OGG) < 0) {
				throw error("Mix_Init failed: %s", Mix_GetError());
			}
		}

		~mixer() {
			Mix_Quit();
		}
	};

	struct sdl {
		core _core;
		mixer _mixer;
		image _image;
		//ttf _ttf;
	};

	struct rect : public SDL_Rect {
		rect() {
			x = 0;
			y = 0;
			w = 0;
			h = 0;
		}
		rect(const rect& o) {
			x = o.x;
			y = o.y;
			w = o.w;
			h = o.h;
		}
		rect(float x, float y, float w, float h) {
			this->x = x;
			this->y = y;
			this->w = w;
			this->h = h;
		}

		bool has_intersection(const rect& r) {
			return SDL_HasIntersection(data(), r.data());
		}

		rect& operator+=(const rect& o) {
			int x2 = (std::min)(x, o.x);
			int y2 = (std::min)(y, o.y);
			int w2 = (std::max)(x + w, o.x + o.w);
			int h2 = (std::max)(y + h, o.y + o.h);
			x = x2;
			y = y2;
			w = w2 - x2;
			h = h2 - y2;
			return *this;
		}

		SDL_Rect* data() const {
			return (SDL_Rect*)this;
		}
	};


	struct point : public SDL_Point {
		point() {
			x = 0;
			y = 0;
		}
		point(float x, float y) {
			this->x = x;
			this->y = y;
		}

		SDL_Point* data() const {
			return (SDL_Point*)this;
		}
	};

	struct screen {
		SDL_Window* window;

		void create(const char* title, int w, int h, int winw, int winh) {
			window = SDL_CreateWindow(title,
			                          SDL_WINDOWPOS_UNDEFINED,
			                          SDL_WINDOWPOS_UNDEFINED,
			                          winw,
			                          winh,
			                          SDL_WINDOW_OPENGL);
			if (window == nullptr) {
				throw error("%s", SDL_GetError());
			}
			SDL_GL_CreateContext(window);
		}

		void present() {
			SDL_GL_SwapWindow(window);
		}

		point get_size() {
			point sz;
			SDL_GetWindowSize(window, &sz.x, &sz.y);
			return sz;
		}
	};


	struct animation {
		enum Type {
			single_shot,
			looping
		};

		enum State {
			stopped,
			playing
		};

		void start() {
			state = playing;
			t = 0.0;
		}

		void stop() {
			state = stopped;
		}

		void update(double dt) {
			if (state == stopped)
				return;
			t += dt;
			if (t > 1.0 / speed) {
				idx++;
				t = 0.0;
			}
			if (idx >= (int)frames.size()) {
				if (type == looping) {
					idx = 0;
				}
				else {
					idx = frames.size() - 1;
					state = stopped;
				}
			}
		}

		int current() const {
			return frames[idx];
		}

		std::vector<int> frames;
		int idx;
		double speed;
		double t;
		Type type;
		State state;
	};


	struct sfx {
		sfx() : _chunk(0) {
		}

		~sfx() {
			if (_chunk)
				Mix_FreeChunk(_chunk);
		}

		void load(const char* filename) {
			_chunk = Mix_LoadWAV(filename);
		}

		void play(int channel) {
			Mix_PlayChannel(channel, _chunk, 0);
		}

		Mix_Chunk* _chunk;
	};

	struct music {
		music() : _music(0) {
		}

		~music() {
			if (_music)
				Mix_FreeMusic(_music);
		}

		void load(const char* filename) {
			_music = Mix_LoadMUS(filename);
		}

		void play() {
			Mix_FadeInMusic(_music, -1, 200);
		}

		Mix_Music* _music;
	};

}
