#include "common.hpp"
#include "render.hpp"
#include "rand.hpp"
#include "assets.hpp"
#include "scene.hpp"

Scene* make_game(Window& screen);


namespace {
	const double TARGET_FPS = 120.0;
	const int SCR_W = 1650;
	const int SCR_H = 960;

	void mainloop(Scene* scene) {
		SDL_Event event;
		scene->init();
		scene->render();

		// http://gafferongames.com/game-physics/fix-your-timestep/
		double t = 0.0;
		const double dt = 1.0 / TARGET_FPS;
		const double freq = 1.0 / (double)SDL_GetPerformanceFrequency();
		double prev_time = (double)SDL_GetPerformanceCounter() * freq;
		double acc = 0.0;
		while (scene->running) {
			double new_time = (double)SDL_GetPerformanceCounter() * freq;
			double frame_time = new_time - prev_time;
			if ( frame_time > 0.25 ) {
				LOG_TRACE("not good %g (%g)", frame_time, acc);
				frame_time = 0.25;	  // note: max frame time to avoid spiral of death
			}
			prev_time = new_time;
			acc += frame_time;
			while (acc >= dt) {
				while (SDL_PollEvent(&event)) {
					scene->handle_event(&event);
				}
				scene->update(dt);

				t += dt;
				acc -= dt;
			}
			scene->render();
		}
	}
}

int main(int argc, char* argv[]) {
	try {
		util::randseed();
		SDL sdl;
		Window screen;
		screen.create("game", SCR_W, SCR_H, SCR_W, SCR_H);
		Assets assets;

		unique_ptr<Scene> game(make_game(screen));
		mainloop(game.get());
	}
	catch (const std::exception& e) {
		LOG_ERROR("%s", e.what());
	}
	return 0;
}
