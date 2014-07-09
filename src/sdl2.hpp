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

	struct sdl {

		sdl() {
			_sdl = _mix = _img = false;
		}

		void init() {
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

			if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
				throw error("SDL_Init failed: %s", SDL_GetError());
			_sdl = true;

			if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) < 0)
				throw error("IMG_Init failed: %s", IMG_GetError());
			_img = true;

			if (Mix_Init(MIX_INIT_OGG) < 0)
				throw error("Mix_Init failed: %s", Mix_GetError());
			_mix = true;
		}

		~sdl() {
			if (_mix) Mix_Quit();
			if (_img) IMG_Quit();
			if (_sdl) SDL_Quit();
		}

		bool _sdl;
		bool _mix;
		bool _img;
	};

	struct screen {
		void create(const char* title, int w, int h, int winw, int winh) {
			window = SDL_CreateWindow(title,
			                          SDL_WINDOWPOS_UNDEFINED,
			                          SDL_WINDOWPOS_UNDEFINED,
			                          winw,
			                          winh,
			                          SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
			if (window == nullptr) {
				throw error("%s", SDL_GetError());
			}
			context = SDL_GL_CreateContext(window);
			if (context == nullptr) {
				throw error("%s", SDL_GetError());
			}
			SDL_GL_MakeCurrent(window, context);

			glewExperimental = GL_TRUE;
			auto glewrc = glewInit();
			if (glewrc != GLEW_OK) {
				throw error("glew error %s \n", glewGetErrorString(glewrc));
			}
			int major, minor;
			glGetIntegerv(GL_MAJOR_VERSION, &major);
			glGetIntegerv(GL_MINOR_VERSION, &minor);
			printf("GL %d.%d\n", major, minor);

			printf("Version: %s\n", glGetString(GL_VERSION));
			printf("Vendor: %s\n", glGetString(GL_VENDOR));
			printf("Renderer: %s\n", glGetString(GL_RENDERER));
			printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		}

		void present() {
			SDL_GL_SwapWindow(window);
		}

		SDL_Point get_size() {
			SDL_Point sz;
			SDL_GetWindowSize(window, &sz.x, &sz.y);
			return sz;
		}

		SDL_Window* window;
		SDL_GLContext context;
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
