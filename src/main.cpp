#include "common.hpp"
#include "render.hpp"
#include "rand.hpp"
#include "world.hpp"
#include "geo.hpp"

namespace {
	const double TARGET_FPS = 120.0;
	const float PI = 3.14159265358979323846f;
	const int SCR_W = 640;
	const int SCR_H = 480;

	constexpr float deg2rad(float deg) {
		return (deg / 360.f) * 2.f * PI;
	}


	struct game {
		game() {
			running = true;
			wireframe_mode = false;
		}

		void init() {
			screen.create("game", SCR_W, SCR_H, SCR_W, SCR_H);

			//Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 512);
			//Mix_AllocateChannels(3);

			SDL_SetRelativeMouseMode(SDL_TRUE);

			init_gl();
			load_shaders();
		}

		void init_gl() {
			glShadeModel(GL_SMOOTH);
			glEnable(GL_CULL_FACE);
			glLogicOp(GL_INVERT);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glCullFace(GL_BACK);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
			glClearColor(0.f, 0.f, 0.f, 0.f);
			glClearDepth(1.f);

			generate_sky();

			make_cube();

			camera.look_at(vec3(3,3,5), vec3(0,0,0));
		}

		void load_shaders() {
			basic_shader.name = load_program("data/basic.vsh", "data/basic.fsh");
		}

		void handle_event(SDL_Event* e) {
			if (e->type == SDL_QUIT) {
				running = false;
			} else if (e->type == SDL_KEYDOWN) {
				if (e->key.keysym.sym == SDLK_w) {
					wireframe_mode = !wireframe_mode;
				}
				else if (e->key.keysym.sym == SDLK_ESCAPE) {
					running  = false;
				}
			}
		}

		int mouse_dx, mouse_dy;
		uint32_t mouse_buttons;

		void tick(double dt) {
			mouse_buttons = SDL_GetRelativeMouseState(&mouse_dx, &mouse_dy);
			//printf("%d, %d, %u\n", x, y, buttons);
		}

		void render() {
			SDL_Point sz = screen.get_size();
			mat4 proj = glm::perspective<float>(deg2rad(50.f), (float)sz.x/(float)sz.y, 0.1, 100.0);
			glViewport(0, 0, sz.x, sz.y);
			glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
			glClearDepth(1.f);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

			glPolygonMode(GL_FRONT, wireframe_mode ? GL_LINE : GL_FILL);

			render_sky();

			glClear(GL_DEPTH_BUFFER_BIT);

			render_cube(proj);

			screen.present();

			SDL_WarpMouseInWindow(screen.window, sz.x >> 1, sz.y >> 1);
		}

		void make_cube() {
			cube.make(vec3(1.f, 1.f, 1.f));
			a = 0.f;
		}

		geo::cube cube;
		float a, b, c;

		void render_cube(const mat4& proj) {
			cube.transform = glm::translate(vec3(sinf(a), cosf(b), sinf(c)));
			cube.transform *= glm::rotate(a*0.6f, vec3(1.f, 0.f, 0.f));
			cube.set_color(vec3(0.3f, 0.f, 0.8f));
			a += 0.04f, b += 0.02f, c += 0.03f;
			cube.render(&basic_shader, proj, camera.pos);
		}

		void generate_sky() {
		}

		void render_sky() {
		}

		renderer screen;
		shader_program basic_shader;
		thing player;
		thing camera;
		bool wireframe_mode;
		bool running;
	} the_game;

	void mainloop() {
		SDL_Event event;
		the_game.init();
		the_game.render();

		// http://gafferongames.com/game-physics/fix-your-timestep/
		double t = 0.0;
		const double dt = 1.0 / TARGET_FPS;

		const double freq = 1.0 / (double)SDL_GetPerformanceFrequency();

		double current_time = (double)SDL_GetPerformanceCounter() * freq;
		double accumulator = 0.0;

		while (the_game.running) {
			double new_time = (double)SDL_GetPerformanceCounter() * freq;
			double frame_time = new_time - current_time;
			if ( frame_time > 0.25 ) {
				LOG_TRACE("not good %g (%g)", frame_time, accumulator);
				frame_time = 0.25;	  // note: max frame time to avoid spiral of death
			}
			current_time = new_time;
			accumulator += frame_time;
			while (accumulator >= dt) {
				while (SDL_PollEvent(&event)) {
					the_game.handle_event(&event);
				}
				the_game.tick(dt);

				t += dt;
				accumulator -= dt;
			}
			the_game.render();
		}
	}
}

int main(int argc, char* argv[]) {
	try {
		util::randseed();
		SDL sdl;
		sdl.init();
		mainloop();
	}
	catch (const std::exception& e) {
		LOG_ERROR("%s", e.what());
	}
	return 0;
}
