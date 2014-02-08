#include "common.hpp"
#include "rand.hpp"

namespace {
	struct game_data {
		SDL_Window* window;
		SDL_Renderer* rnd;
		bool running;

		SDL_Texture* screen;
		Uint32* pixels;

		game_data() {
			window = nullptr;
			rnd = nullptr;
			running = true;
			pixels = new Uint32[320 * 200];
		}

		~game_data() {
			delete [] pixels;
		}
	};

	struct sprite {
		
	};

	game_data game;

	void init() {
		game.window = SDL_CreateWindow("simplex",
		                               SDL_WINDOWPOS_UNDEFINED,
		                               SDL_WINDOWPOS_UNDEFINED,
		                               960,
		                               600,
		                               0);
		if (game.window == nullptr) {
			throw sdl::error("%s", SDL_GetError());
		}
		// | SDL_RENDERER_PRESENTVSYNC
		game.rnd = SDL_CreateRenderer(game.window, -1,
		                              SDL_RENDERER_ACCELERATED);
		if (game.rnd == nullptr){
			throw sdl::error("%s", SDL_GetError());
		}
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
		SDL_RenderSetLogicalSize(game.rnd, 320, 200);

		game.screen = SDL_CreateTexture(game.rnd,
		                                SDL_PIXELFORMAT_ARGB8888,
		                                SDL_TEXTUREACCESS_STREAMING,
		                                320, 200);

		for (int y = 0; y < 200; ++y) {
			for (int x = 0; x < 320; ++x) {
				game.pixels[(199 - y)*320 + x] = 0xff101010 + (23 + y);
			}
		}
		for (int i = 0; i < 64; ++i) {
			uint64_t rnd = util::randint();
			int x = (rnd >> 32) % 320;
			int y = rnd % 200;
			game.pixels[y*320 + x] = 0xffffffff;
		}
	}

	void handle_event(SDL_Event* e) {
		if (e->type == SDL_QUIT) {
			game.running = false;
		}
		else if (e->type == SDL_KEYDOWN) {
			if (e->key.keysym.sym == SDLK_ESCAPE) {
				game.running = false;
			}
		}
	}

	void tick() {
	}

	void render() {
		SDL_UpdateTexture(game.screen, NULL, game.pixels, 320 * sizeof(Uint32));
		SDL_RenderCopy(game.rnd, game.screen, NULL, NULL);
		SDL_RenderPresent(game.rnd);
	}

	void mainloop() {
		SDL_Event event;
		Uint32 counter = 0;
		while (game.running) {
			counter = SDL_GetTicks();
			while (SDL_PollEvent(&event)) {
				handle_event(&event);
			}
			tick();
			render();
			sdl::delay_to_fps(counter);
		}
	}

}

int main(int argc, char* argv[]) {
	LOG_INFO("Info test");
	LOG_WARN("Warning test");
	LOG_ERROR("Error test");
	LOG_TRACE("Trace test");
	try {
		util::randseed();
		sdl::sdl lib;

		init();
		render();
		mainloop();
	}
	catch (const std::exception& e) {
		LOG_ERROR("%s", e.what());
	}
	return 0;
}
