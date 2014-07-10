#include "common.hpp"
#include "render.hpp"
#include "rand.hpp"
#include "world.hpp"
#include "geo.hpp"
#include "aabb.hpp"
#include "assets.hpp"

namespace {
	const double TARGET_FPS = 120.0;
	const float PI = 3.14159265358979323846f;
	const int SCR_W = 640;
	const int SCR_H = 480;

	constexpr float deg2rad(float deg) {
		return (deg / 360.f) * 2.f * PI;
	}

	struct Player {

		void move(const glm::vec3& dir) {
			mat4 ori = glm::rotate(mat4(), angle, vec3(0.f, 1.f, 0.f));
			pos += vec3(vec4(dir, 1.f) * ori);
		}

		void looky(float y) {
			const float twopi = PI*2.f;
			angle += y;
			while (angle < 0.f)
				angle += twopi;
			while (angle > twopi)
				angle -= twopi;
		}

		void jump() {
		}

		vec3 pos;
		float angle; // look angle around Y axis (radians)
	};

	struct Camera {
		// position and orientation of camera
		// relative to player
		// an fps camera is offset from the player position
		// and rotates with the player in the XZ direction
		// it can then rotate in a limited range around its
		// local Z axis
		vec3 offset;
		float angle; // look angle around Z axis (radians)

		void lookz(float z) {
			const float halfpi = PI*0.5f;
			angle += z;
			if (angle > halfpi)
				angle = halfpi;
			if (angle < -halfpi)
				angle = -halfpi;
		}
	};

	// calculate view matrix
	mat4 make_view_matrix(const Player& player, const Camera& camera) {
		mat4 view;
		view = glm::rotate(view, camera.angle, vec3(1.f, 0.f, 0.f));
		view = glm::rotate(view, player.angle, vec3(0.f, 1.f, 0.f));
		view = glm::translate(view, player.pos - camera.offset);
		return view;
	}

	// the sky view matrix
	mat4 make_sky_view_matrix(const Player& player, const Camera& camera) {
		mat4 view;
		view = glm::rotate(view, camera.angle, vec3(1.f, 0.f, 0.f));
		view = glm::rotate(view, player.angle, vec3(0.f, 1.f, 0.f));
		return view;
	}

	struct Game {
		Game() {
			running = true;
			wireframe_mode = false;
		}

		~Game() {
			release_assets();
		}

		void init() {
			screen.create("game", SCR_W, SCR_H, SCR_W, SCR_H);

			//Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 512);
			//Mix_AllocateChannels(3);

			SDL_SetRelativeMouseMode(SDL_TRUE);

			init_gl();
			load_shaders();

			player.pos = vec3(3.f, 0.f, 3.f);
			player.angle = 0.f;
			camera.offset = vec3(0.f, 1.85f, 0.f);
			camera.angle = 0.f;
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

			make_sky();
			make_groundplane();
			make_cube();

			//player.look_at(vec3(3,3,5), vec3(0,0,0));
		}

		void load_shaders() {
			material_basic = load_material("basic");
			material_sky = load_material("sky");
		}

		void handle_event(SDL_Event* e) {
			if (e->type == SDL_QUIT) {
				running = false;
			} else if (e->type == SDL_KEYDOWN) {
				switch (e->key.keysym.sym) {
				case SDLK_F1: {
					wireframe_mode = !wireframe_mode;
				} break;
				case SDLK_ESCAPE: {
					running  = false;
				} break;
				}
			}
		}

		void handle_input(double dt) {
			float speed = 6.f * dt;

			const Uint8* state = SDL_GetKeyboardState(NULL);
			if (state[SDL_SCANCODE_A])
				player.move(vec3(speed, 0.f, 0.f));
			if (state[SDL_SCANCODE_D])
				player.move(vec3(-speed, 0.f, 0.f));
			if (state[SDL_SCANCODE_W])
				player.move(vec3(0.f, 0.f, speed));
			if (state[SDL_SCANCODE_S])
				player.move(vec3(0.f, 0.f, -speed));

			int mouse_dx, mouse_dy;
			uint32_t mouse_buttons;
			mouse_buttons = SDL_GetRelativeMouseState(&mouse_dx, &mouse_dy);
			float xsens = 1.f/(2.f*PI);
			float ysens = 1.f/(2.f*PI);

			if (mouse_dx != 0)
				player.looky(mouse_dx * dt * xsens);
			if (mouse_dy != 0)
				camera.lookz(mouse_dy * dt * ysens);
		}


		void tick(double dt) {
			//printf("%d, %d, %u\n", x, y, buttons);

			update_daycycle(dt);
			update_chunks(dt);
			update_ecosystem(dt);
			update_player(dt);
			update_creatures(dt);
		}

		void update_daycycle(double dt) {
		}
		void update_chunks(double dt) {
		}
		void update_ecosystem(double dt) {
		}
		void update_player(double dt) {
		}
		void update_creatures(double dt) {
		}

		void render() {
			SDL_Point sz = screen.get_size();
			proj = glm::perspective<float>(deg2rad(50.f), (float)sz.x/(float)sz.y, 0.1, 100.0);
			glViewport(0, 0, sz.x, sz.y);
			glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
			glClearDepth(1.f);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

			glPolygonMode(GL_FRONT, wireframe_mode ? GL_LINE : GL_FILL);

			view = make_view_matrix(player, camera);

			render_sky();

			glClear(GL_DEPTH_BUFFER_BIT);

			render_groundplane();

			render_cube();

			screen.present();

			SDL_WarpMouseInWindow(screen.window, sz.x >> 1, sz.y >> 1);
		}

		void make_cube() {
			cube.make(vec3(0.5f, 0.5f, 0.5f));
			a = 0.f;
		}

		void make_groundplane() {
			plane.make();
		}

		mat4 proj;
		mat4 view;


		void render_groundplane() {
			glVertexAttrib3f(2u, 0.2f, 0.6f, 0.2f);
			plane.render(material_basic, proj, view);
		}

		void render_cube() {
			cube.transform = glm::rotate(a*0.1f, vec3(0.f, 1.f, 0.f)) * glm::translate(vec3(0.f, 1.f, 0.f));
			a += 0.04f, b += 0.02f, c += 0.03f;

			Material* material = material_basic;
			material->use();
			material->uniform("projection", proj);
			material->uniform("view", view);
			material->uniform("model", cube.transform);

			glVertexAttrib3f(2u, 0.3f, 0.0f, 0.8f);
			cube.render(proj, view);
		}

		void make_sky() {
			sky.make(vec3(10.f, 10.f, 10.f), true);
		}

		void render_sky() {
			mat4 skyview = make_sky_view_matrix(player, camera);
			glVertexAttrib3f(2u, 1.f, 1.f, 1.f);

			Material* material = material_sky;
			material->use();
			material->uniform("projection", proj);
			material->uniform("view", skyview);
			material->uniform("model", sky.transform);
			material->uniform("sky_dark", vec3(0.3f, 0.3f, 0.6f));
			material->uniform("sky_light", vec3(0.5f, 0.5f, 0.8f));
			sky.render(proj, skyview);
		}

		Window screen;
		Material* material_basic;
		Material* material_sky;
		Player player;
		Camera camera;
		geo::plane plane;
		geo::cube cube;
		geo::cube sky;
		float a, b, c;
		bool wireframe_mode;
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

		double current_time = (double)SDL_GetPerformanceCounter() * freq;
		double accumulator = 0.0;

		while (game.running) {
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
					game.handle_event(&event);
				}
				game.handle_input(dt);
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
		SDL sdl;
		sdl.init();
		mainloop();
	}
	catch (const std::exception& e) {
		LOG_ERROR("%s", e.what());
	}
	return 0;
}
