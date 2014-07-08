#include "common.hpp"
#include "rand.hpp"
#include <deque>
#include <algorithm>
#include <math.h>

namespace {
	const double TARGET_FPS = 120.0;
	const float PI = 3.14159265358979323846f;
	const int SCR_W = 920;
	const int SCR_H = 480;
	const int CHUNK_SIZE = 16;
	const int INITIAL_CHUNK_CACHE_SIZE_MB = 64;

	struct Player {
		double x, y, z;
	};

	typedef uint8_t Block;

	// for 16^3 chunks and 1 byte/voxel = 16k chunks in cache if 64MB chunk cache
	const int INITIAL_CHUNK_CACHE_SIZE = (INITIAL_CHUNK_CACHE_SIZE_MB * 1024 * 1024)
		/ (sizeof(Block) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);

	struct Chunk {
		uint32_t id;
		int16_t x, y, z;
		uint16_t flags;
		// voxel data for chunk
		Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
	};

	// the chunk cache has a maximum size
	// and background loader threads
	struct ChunkCache {
		Chunk cache[INITIAL_CHUNK_CACHE_SIZE];
	};

	struct World {
		uint64_t seed;
	};

	struct Game {
		Game() {
			running = true;
		}

		void init() {
			screen.create("game", SCR_W, SCR_H, SCR_W, SCR_H);

			Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 512);
			Mix_AllocateChannels(3);

			SDL_SetRelativeMouseMode(SDL_TRUE);

			init_gl();
		}

		void init_gl() {
			glShadeModel(GL_SMOOTH);
			glEnable(GL_CULL_FACE);
			glLogicOp(GL_INVERT);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
			glClearColor(0.f, 0.f, 0.f, 0.f);
			glClearDepth(1.f);

			generate_sky();
		}

		void handle_event(SDL_Event* e) {
			if (e->type == SDL_QUIT) {
				running = false;
			} else if (e->type == SDL_KEYDOWN) {
				running  = false;
			}
		}

		int mouse_dx, mouse_dy;
		Uint32 mouse_buttons;

		void tick(double dt) {
			mouse_buttons = SDL_GetRelativeMouseState(&mouse_dx, &mouse_dy);
			//printf("%d, %d, %u\n", x, y, buttons);
		}

		void render() {
			sdl::point sz = screen.get_size();
			glViewport(0, 0, sz.x, sz.y);
			glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
			glClearDepth(1.f);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

			render_sky();

			glClear(GL_DEPTH_BUFFER_BIT);

			screen.present();

			SDL_WarpMouseInWindow(screen.window, sz.x >> 1, sz.y >> 1);
		}

		void generate_sky() {
		}

		void render_sky() {
		}

		sdl::screen screen;

		bool running;
	} game;

	void mainloop() {
		SDL_Event event;
		game.init();
		game.render();

		// http://gafferongames.com/game-physics/fix-your-timestep/
		double t = 0.0;
		const double dt = 1.0 / TARGET_FPS;

		const double freq = 1.0 / (double)SDL_GetPerformanceFrequency();

		double currentTime = (double)SDL_GetPerformanceCounter() * freq;
		double accumulator = 0.0;

		while (game.running) {
			double newTime = (double)SDL_GetPerformanceCounter() * freq;
			double frameTime = newTime - currentTime;
			if ( frameTime > 0.25 ) {
				LOG_TRACE("eh %g (%g)", frameTime, accumulator);
				frameTime = 0.25;	  // note: max frame time to avoid spiral of death
			}
			currentTime = newTime;
			accumulator += frameTime;
			while (accumulator >= dt) {
				while (SDL_PollEvent(&event)) {
					game.handle_event(&event);
				}
				game.tick(dt);

				t += dt;
				accumulator -= dt;
			}
			game.render();
		}
	}
}

int main(int argc, char* argv[]) {
	try {
		util::randseed();
		sdl::sdl lib;
		mainloop();
	}
	catch (const std::exception& e) {
		LOG_ERROR("%s", e.what());
	}
	return 0;
}
