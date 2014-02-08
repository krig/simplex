#pragma once
#include "common.hpp"

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

	inline Uint32 delay_to_fps(const Uint32 counter,
	                           const Uint32 min_ticks_per_frame = (Uint32)((1.0/ 60.0) * 1000.0)) {
		const Uint32 counter2 = SDL_GetTicks();
		if ((counter2 > counter) && ((counter2 - counter) < min_ticks_per_frame)) {
			SDL_Delay(min_ticks_per_frame - (counter2 - counter));
		}
		return counter2;
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

}
