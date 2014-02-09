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
	                           const Uint32 min_ticks_per_frame = (Uint32)((1.0 / 60.0) * 1000.0)) {
		const Uint32 counter2 = SDL_GetTicks();
		const Uint32 delta = counter2 - counter;
		if (delta < min_ticks_per_frame) {//(counter2 > counter) && ((counter2 - counter) < min_ticks_per_frame)) {
			SDL_Delay(min_ticks_per_frame - delta);
		}
		else {
			LOG_TRACE("%u / %u", counter2 - counter, min_ticks_per_frame);
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
		SDL_Renderer* rnd;

		void create(int w, int h, int winw, int winh) {
			window = SDL_CreateWindow("simplex",
			                          SDL_WINDOWPOS_UNDEFINED,
			                          SDL_WINDOWPOS_UNDEFINED,
			                          winw,
			                          winh,
			                          0);
			if (window == nullptr) {
				throw error("%s", SDL_GetError());
			}
			// | SDL_RENDERER_PRESENTVSYNC
			rnd = SDL_CreateRenderer(window, -1,
			                         SDL_RENDERER_ACCELERATED);
			if (rnd == nullptr){
				throw error("%s", SDL_GetError());
			}
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
			SDL_RenderSetLogicalSize(rnd, w, h);
		}

		void present() {
			SDL_RenderPresent(rnd);
		}
	};

	struct sprite {
		void load(screen& s, const char* filename) {
			SDL_Surface* img = IMG_Load(filename);
			if (img == nullptr) {
				throw error("Failed to load %s: %s", filename, SDL_GetError());
			}
			_texture = SDL_CreateTextureFromSurface(s.rnd, img);
			if (_texture == nullptr) {
				throw error("Failed to load %s: %s", filename, SDL_GetError());
			}
			if (SDL_QueryTexture(_texture, &format, &access, &w, &h) < 0) {
				throw error("Failed to load %s: %s", filename, SDL_GetError());
			}
			LOG_TRACE("%s (%d, %d)", filename, w, h);
		}
		void draw(screen& s, const rect& src, const rect& dst) {
			SDL_RenderCopy(s.rnd, _texture, src.data(), dst.data());
		}

		void draw(screen& s) {
			SDL_RenderCopy(s.rnd, _texture, NULL, NULL);
		}

		void draw(screen& s, const rect& dst) {
			SDL_RenderCopy(s.rnd, _texture, NULL, dst.data());
		}

		void draw_angle(screen& s, const rect& dst, double angle, const point* p) {
			SDL_RenderCopyEx(s.rnd, _texture, NULL, dst.data(),
			                 angle, (const SDL_Point*)p, SDL_FLIP_NONE);
		}

		~sprite() {
			if (_texture) {
				SDL_DestroyTexture(_texture);
			}
		}
		SDL_Texture* _texture;
		Uint32 format;
		int access;
		int w;
		int h;
	};

	struct animated_sprite : public sprite {
		void init_anim(int frames, int ticks_per_frame) {
			this->frames = frames;
			this->ticks_per_frame = ticks_per_frame;
			dt = 0;
			currframe = 0;
		}

		void update() {
			dt += 1;
			if (dt > ticks_per_frame) {
				dt = 0;
				currframe += 1;
				if (currframe >= frames)
					currframe = 0;
			}
		}

		rect framerect() {
			int y = frame_h() * currframe;
			return rect(0, y, w, frame_h());
		}

		int frame_h() {
			return h / frames;
		}

		void draw_angle(screen& s, const rect& dst, double angle, const point* p) {
			rect f = framerect();
			SDL_RenderCopyEx(s.rnd, _texture, f.data(), dst.data(),
			                 angle, (const SDL_Point*)p, SDL_FLIP_NONE);
		}

		int frames;
		int currframe;
		int dt;
		int ticks_per_frame;
	};

	struct numbers {
		void load(screen& s, const char* filename) {
			img.load(s, filename);
		}

		int _count(int number) const {
			int np = 1;
			while (number > 9) {
				number /= 10;
				np += 1;
			}
			return np;
		}

		void draw(screen& s, int number, const point& pos) {
			int np = _count(number);
			int nw = img.w / 10;
			int tw = np * nw;
			int x = pos.x + (tw / 2) - (nw);
			while (true) {
				int n = number % 10;

				rect src(nw * n, 0, nw, img.h);
				rect dst(x, pos.y - img.h / 2, nw, img.h);
				img.draw(s, src, dst);
				if (number < 10)
					break;
				number /= 10;
				x -= nw;
			}
		}

		sprite img;
	};

}
