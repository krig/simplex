#include "common.hpp"
#include "rand.hpp"
#include <deque>
#include <algorithm>
#include <math.h>

namespace {
	const double TARGET_FPS = 60.0;
	const float PI = 3.14159265358979323846f;
	const float SCR_W = 320.f;
	const float SCR_H = 240.f;

	struct Game {
		Game() {
			running = true;
		}

		void init() {
			screen.create("game", (int)SCR_W, (int)SCR_H, 640, 480);

			Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 512);
			Mix_AllocateChannels(3);

		}

		void handle_event(SDL_Event* e) {
			if (e->type == SDL_QUIT) {
				running = false;
			} else if (e->type == SDL_KEYDOWN) {
				running  = false;
			}
		}

		void tick(double dt) {
		}

		void render() {
			screen.present();
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
