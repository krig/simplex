#include "common.hpp"
#include "render.hpp"
#include "rand.hpp"
#include "world.hpp"
#include "geo.hpp"
#include "aabb.hpp"
#include "assets.hpp"
#include "player.hpp"
#include "matrixstack.hpp"

namespace {
	const double TARGET_FPS = 120.0;
	const int SCR_W = 960;
	const int SCR_H = 480;

	mat4 fps_view(vec3 eye, float pitch, float yaw) {
		float cosPitch = cosf(pitch);
		float sinPitch = sinf(pitch);
		float cosYaw = cosf(yaw);
		float sinYaw = sinf(yaw);
		vec3 xaxis(cosYaw, 0, -sinYaw);
		vec3 yaxis(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
		vec3 zaxis(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

		mat4 viewMatrix = {
			vec4(       xaxis.x,            yaxis.x,            zaxis.x,      0 ),
			vec4(       xaxis.y,            yaxis.y,            zaxis.y,      0 ),
			vec4(       xaxis.z,            yaxis.z,            zaxis.z,      0 ),
			vec4( -glm::dot( xaxis, eye ), -glm::dot( yaxis, eye ), -glm::dot( zaxis, eye ), 1 )
		};
		return viewMatrix;
	}

	// calculate world -> view matrix
	mat4 make_view_matrix(const Player& player) {
		return fps_view(player.pos + player.offset + player.bob,
		                player.pitch, player.yaw);
	}

	// calculate world -> view matrix with no translation
	mat4 make_sky_view_matrix(const Player& player) {
		return fps_view(vec3(0, 0, 0), player.pitch, player.yaw);
	}

	struct Game {
		Game() {
			running = true;
			wireframe_mode = false;
		}

		~Game() {
		}

		void init() {
			screen.create("game", SCR_W, SCR_H, SCR_W, SCR_H);

			//Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 512);
			//Mix_AllocateChannels(3);

			SDL_SetRelativeMouseMode(SDL_TRUE);
			mouse_captured = true;

			init_gl();
			load_shaders();

			player.pos = vec3(0.f, 0.f, 5.f);
			player.offset = vec3(0.f, 1.85f, 0.f);
			player.pitch = player.yaw = 0.f;

			world.init(util::randint());
		}

		void init_gl() {
			glShadeModel(GL_SMOOTH);
			glEnable(GL_CULL_FACE);
			glLogicOp(GL_INVERT);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glCullFace(GL_BACK);
			glEnable(GL_TEXTURE_2D);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
			glClearColor(0.f, 0.f, 0.f, 0.f);
			glClearDepth(1.f);

			make_sky();
			make_groundplane();
			make_cubes();

			//player.look_at(vec3(3,3,5), vec3(0,0,0));

			SDL_Point sz = screen.get_size();
			projection.load(glm::perspective<float>(deg2rad(50.f), (float)sz.x/(float)sz.y, 0.1, 100.0));
			glViewport(0, 0, sz.x, sz.y);
		}

		void load_shaders() {
			material_basic = load_material("basic");
			material_sky = load_material("sky");
			cube_tex = load_texture("data/test64.png");
		}

		void handle_event(SDL_Event* e) {
			switch (e->type) {
			case SDL_QUIT: {
				running = false;
			} break;
			case SDL_KEYDOWN: {
				switch (e->key.keysym.sym) {
				case SDLK_F1: {
					wireframe_mode = !wireframe_mode;
				} break;
				case SDLK_ESCAPE: {
					running  = false;
				} break;
				}
			} break;
			case SDL_WINDOWEVENT: {
				switch (e->window.event) {
				case SDL_WINDOWEVENT_RESIZED: {
					SDL_Point sz = { e->window.data1, e->window.data2 };
					projection.load(glm::perspective<float>(deg2rad(50.f), (float)sz.x/(float)sz.y, 0.1, 100.0));
					glViewport(0, 0, sz.x, sz.y);
				} break;
				case SDL_WINDOWEVENT_FOCUS_GAINED: {
				} break;
				case SDL_WINDOWEVENT_FOCUS_LOST: {
					SDL_SetRelativeMouseMode(SDL_FALSE);
					mouse_captured = false;
				} break;
				case SDL_WINDOWEVENT_CLOSE: {
					running = false;
				} break;
				}
			} break;
			}
		}

		void handle_input(double dt) {
			float speed = 2.f;

			const Uint8* state = SDL_GetKeyboardState(NULL);
			if (state[SDL_SCANCODE_A])
				player.move(vec3(-speed, 0.f, 0.f));
			if (state[SDL_SCANCODE_D])
				player.move(vec3(speed, 0.f, 0.f));
			if (state[SDL_SCANCODE_W])
				player.move(vec3(0.f, 0.f, -speed));
			if (state[SDL_SCANCODE_S])
				player.move(vec3(0.f, 0.f, speed));
			if (state[SDL_SCANCODE_SPACE])
				player.jump();

			if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_S]) {
				player.bob += vec3(sinf(bobd.x) * 0.003f, cosf(bobd.y) * 0.006f, 0.f);
				bobd.x += speed * 1.6f;
				bobd.y += speed * 1.8f;
			} else {
				bobd = vec3(0, 0, 0);
			}

			if (mouse_captured) {
				int mouse_dx, mouse_dy;
				uint32_t mouse_buttons;
				mouse_buttons = SDL_GetRelativeMouseState(&mouse_dx, &mouse_dy);
				(void)mouse_buttons;
				float xsens = 1.f / TWOPI;
				float ysens = 1.f / TWOPI;

				player.look(mouse_dx * dt * xsens, mouse_dy * dt * ysens);
			} else if (SDL_GetMouseState(0, 0) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
				mouse_captured = true;
				SDL_SetRelativeMouseMode(SDL_TRUE);
			}
		}


		void tick(double dt) {
			//printf("%d, %d, %u\n", x, y, buttons);

			update_assets();
			update_daycycle(dt);
			update_chunks(dt);
			update_ecosystem(dt);
			player.tick(dt);
			update_creatures(dt);
		}

		void update_daycycle(double dt) {
		}
		void update_chunks(double dt) {
		}
		void update_ecosystem(double dt) {
		}
		void update_creatures(double dt) {
		}

		void render() {
			SDL_Point sz = screen.get_size();
			glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
			glClearDepth(1.f);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

			glPolygonMode(GL_FRONT, wireframe_mode ? GL_LINE : GL_FILL);

			modelview.load(make_view_matrix(player));

			render_sky();

			glClear(GL_DEPTH_BUFFER_BIT);

			render_groundplane();

			world.render();

			render_cubes();

			screen.present();

			if (mouse_captured) {
				SDL_WarpMouseInWindow(screen.window, sz.x >> 1, sz.y >> 1);
			}
		}

		void make_cubes() {
			{
				geo::cube cube1;
				cube1.make(vec3(0.5f, 0.5f, 0.5f));
				cube1.transform = glm::translate(vec3(0.f, 0.5f, 0.f));
				cubes.emplace_back(cube1);
			}

			for (int i = -10; i < 10; ++i) {
				geo::cube cube1;
				cube1.make(vec3(0.25f, 0.25f, 0.25f));
				cube1.transform = glm::translate(vec3(1.f + i, 0.5f, 0.f));
				cubes.emplace_back(cube1);
			}

			for (int i = -10; i < 10; ++i) {
				geo::cube cube1;
				cube1.make(vec3(0.25f, 0.25f, 0.25f));
				cube1.transform = glm::translate(vec3(0.f, 0.5f, 1.f + i));
				cubes.emplace_back(cube1);
			}

			a = 0.f;
		}

		void make_groundplane() {
			plane.make();
		}

		void render_groundplane() {
			modelview.push();
			Material* material = material_basic;
			cube_tex->bind(0);
			material->use();
			material->uniform("tex0", 0);
			material->uniform("MVP", projection.get() * modelview.get());
			material->uniform("normal_matrix", mat3());
			plane.render();
			modelview.pop();
		}

		void render_cubes() {
			a += 0.04f, b += 0.02f, c += 0.03f;

			Material* material = material_basic;
			cube_tex->bind(0);
			material->use();
			material->uniform("tex0", 0);

			for (auto& cube : cubes) {
				modelview.push();
				modelview *= cube.transform;
				material->uniform("MVP", projection.get() * modelview.get());
				material->uniform("normal_matrix", mat3(cube.transform));
				cube.render();
				modelview.pop();
			}
		}

		void make_sky() {
			sky.make(vec3(5.f, 5.f, 5.f), true);
		}

		void render_sky() {
			mat4 skyview = make_sky_view_matrix(player);

			Material* material = material_sky;
			material->use();
			material->uniform("projection", projection.get());
			material->uniform("view", skyview);
			material->uniform("model", sky.transform);
			material->uniform("sky_dark", vec3(0.3f + 0.7f*player.jumpcount, 0.333f, 0.4f));
			material->uniform("sky_light", vec3(0.86f, 0.8f, 0.91f));
			sky.render();
		}

		Window screen;

		bool mouse_captured;

		MatrixStack<8> projection;
		MatrixStack<32> modelview;

		Player player;

		World world;

		Texture2D* cube_tex;
		Material* material_basic;
		Material* material_sky;
		geo::plane plane;
		std::vector<geo::cube> cubes;
		geo::cube sky;
		vec3 bobd;
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
		Assets assets;
		SDL sdl;
		sdl.init();
		mainloop();
	}
	catch (const std::exception& e) {
		LOG_ERROR("%s", e.what());
	}
	return 0;
}
