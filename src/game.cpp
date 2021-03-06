#include "common.hpp"
#include "render.hpp"
#include "scene.hpp"
#include "geo.hpp"
#include "aabb.hpp"
#include "assets.hpp"
#include "rand.hpp"
#include "colors.hpp"
#include "player.hpp"
#include "matrixstack.hpp"
#include "worley.hpp"
#include "ui.hpp"
#include "world.hpp"

namespace {
	float mix(float a, float b, float t) {
		return (1.f - t) * a + t * b;
	}
	Color mix(Color a, Color b, float t) {
		return Color(mix(a.x, b.x, t),
		             mix(a.y, b.y, t),
		             mix(a.z, b.z, t),
		             mix(a.w, b.w, t));
	}

	float fade(float a, float b, float t) {
		return t * t * t * (t * (t * 6 - 15.f) + 10.f);
	}

	float clamp(float v, float l = 0.f, float h = 1.f) {
		return v > h ? h : v < l ? l : v;
	}

	template <size_t NOctaves>
	float octave_simplex(float freq, float lacunarity, float x, float y, float t) {
		float v = 0.f;
		float amp = 0.5f;
		for (size_t i = 0; i < NOctaves; ++i) {
			v += amp * glm::simplex(vec3(x * freq, y * freq, t));
			freq *= lacunarity;
			amp /= lacunarity;
		}
		return v;
	}

	template <size_t NOctaves>
	float octave_perlin(float freq, float lacunarity, float period, float x, float y, float t) {
		float v = 0.f;
		float amp = 0.5f;
		for (size_t i = 0; i < NOctaves; ++i) {
			v += amp * glm::perlin(vec2(x * freq, y * freq), vec2(freq, freq));
			freq *= lacunarity;
			amp /= lacunarity;
		}
		return v;
	}

}

struct Game : public Scene {
	Game(Window& screen) : Scene(), screen(screen) {
		wireframe_mode = false;
	}

	~Game() {
	}

	void init() {

		//Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 512);
		//Mix_AllocateChannels(3);

		SDL_SetRelativeMouseMode(SDL_TRUE);
		mouse_captured = true;

		init_gl();
		load_shaders();
		ui.init();

		player.pos = vec3(0.f, 0.f, 5.f);
		player.offset = vec3(0.f, 1.85f, 0.f);
		player.pitch = player.yaw = 0.f;
	}

	void init_gl() {
		//glLineWidth(2.f);
		glEnable(GL_CULL_FACE);
		glLogicOp(GL_INVERT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glCullFace(GL_BACK);
		glEnable(GL_TEXTURE_2D);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClearDepth(1.f);

		make_sky();
		make_groundplane();
		make_cubes();
		world::init();

		SDL_Point sz = screen.get_size();
		projection.load(glm::perspective<float>(deg2rad(50.f), (float)sz.x/(float)sz.y, 0.1, 16.0 * 32.0));
		glViewport(0, 0, sz.x, sz.y);
	}

	void load_shaders() {
		material_basic = load_material("basic");
		material_sky = load_material("sky");
		//cube_tex = load_texture("data/test8.png");
		cube_tex = gen_texture("cubetex", 32, 32, [](int x, int y) {
				//float n = worley_noise(glm::dvec3((double)x / 64., (double)y / 64., 1.), 1, 3.);
				double d[2];
				double scale = 32. / 4.f;
				worley_noise(glm::dvec3((double)x / scale, (double)y / scale, 1.), d, 2, scale);
				float n = d[1] - d[0];
				return mix(colors::dawnbringer::black, Color(0xffffffff), n);
			});
		double d = 10000. * util::rand01();
		float freq = 16.f;
		float lacunarity = 2.17f;
		float side = 32.f;
		plane_tex = gen_texture("planetex", side, side, [=](int x, int y) {
				float n = octave_perlin<3>(freq, lacunarity, side, (float)x / side, (float)y / side, d);
				n = (n + 1.f) * 0.5f;
				return mix(Color(0xff144504), colors::dawnbringer::dark_green, n);
			});
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
			case SDLK_F2: {
				player.toggle_freecam();
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

		player.duck(state[SDL_SCANCODE_LSHIFT] != 0);

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


	void update(double dt) {
		//printf("%d, %d, %u\n", x, y, buttons);
		handle_input(dt);
		update_assets();

		world::update(dt);

		update_daycycle(dt);
		update_chunks(dt);
		update_ecosystem(dt);
		player.update(dt);
		update_creatures(dt);
	}

	void update_daycycle(double dt) {
		double daylength = 1.0 / (60.0 * 5.0);
		a += daylength * dt;
		while (a > 1.f)
			a -= 1.f;
	}
	void update_chunks(double dt) {
	}
	void update_ecosystem(double dt) {
	}
	void update_creatures(double dt) {
	}

	void render() {
		SDL_Point sz = screen.get_size();
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClearDepth(1.f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		if (wireframe_mode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		modelview.load(player.make_view_matrix());

		render_groundplane();
		render_cubes();
		world::render();

		render_sky();

		if (wireframe_mode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		render_ui(sz);

		screen.present();

		if (mouse_captured) {
			SDL_WarpMouseInWindow(screen.window, sz.x >> 1, sz.y >> 1);
		}

		GLenum err = glGetError();
		while (err != GL_NO_ERROR) {
			LOG_ERROR("GL error: %d", err);
			err = glGetError();
		}
	}

	void render_ui(SDL_Point sz) {
		ui.begin(sz);

		vec2 mid(sz.x/2, sz.y/2);

		//ui.rect(mid - vec2(50.f, 50.f), mid + vec2(50.f, 50.f),
		//        colors::dawnbringer::black, true);
		//ui.rect(mid - vec2(50.f, 50.f), mid + vec2(50.f, 50.f),
		//        colors::dawnbringer::yellow, false);

		ui.line(mid - vec2(0, 16.f), mid + vec2(0, 16.f), colors::flatui::asbestos.with_alpha(0.6f));
		ui.line(mid - vec2(16.f, 0), mid + vec2(16.f, 0), colors::flatui::asbestos.with_alpha(0.6f));

		ui.text(vec2(16.f, 16.f), Color(0xffffffff), "%f",
		        a);

		ui.end();
	}

	void make_cubes() {
		typedef unique_ptr<geo::instance> cube_ptr;
		{
			cube_ptr cube1(new geo::instance);
			geo::make_cube(cube1.get(), vec3(0.5f, 0.5f, 0.5f));
			cube1->transform = glm::translate(vec3(0.f, 0.5f, 0.f));
			cubes.push_back(move(cube1));
		}

		for (int i = -10; i < 10; ++i) {
			cube_ptr cube1(new geo::instance);
			geo::make_cube(cube1.get(), vec3(0.25f, 0.25f, 0.25f));
			cube1->transform = glm::translate(vec3(1.f + i, 0.5f, 0.f));
			cubes.push_back(move(cube1));
		}

		for (int i = -10; i < 10; ++i) {
			cube_ptr cube1(new geo::instance);
			geo::make_cube(cube1.get(), vec3(0.25f, 0.25f, 0.25f));
			cube1->transform = glm::translate(vec3(0.f, 0.5f, 1.f + i));
			cubes.push_back(move(cube1));
		}

		a = 0.f;
	}

	void make_groundplane() {
		geo::make_plane(&plane);
	}

	void render_groundplane() {
		modelview.push();
		Material* material = material_basic;
		plane_tex->bind(0);
		material->use();
		material->uniform("tex0", 0);
		material->uniform("MV", modelview.get());
		material->uniform("MVP", projection.get() * modelview.get());
		material->uniform("normal_matrix", mat3());
		plane.render();
		modelview.pop();
	}

	void render_cubes() {
		Material* material = material_basic;
		cube_tex->bind(0);
		material->use();
		material->uniform("tex0", 0);

		for (const auto& cube : cubes) {
			modelview.push();
			modelview *= cube->transform;
			material->uniform("MV", modelview.get());
			material->uniform("MVP", projection.get() * modelview.get());
			material->uniform("normal_matrix", mat3(cube->transform));
			cube->render();
			modelview.pop();
		}
	}

	void make_sky() {
		//geo::make_cone(&sky, 5.f, 5.f, 31, true);
		geo::make_sphere(&sky, 5.f, 2, true);
	}

	void render_sky() {
		glDepthFunc(GL_EQUAL);
		glDepthRange(1, 1);

		mat4 skyview = player.make_sky_view_matrix();
		Material* material = material_sky;
		material->use();
		material->uniform("projection", projection.get());
		material->uniform("view", skyview);
		material->uniform("model", sky.transform);
		vec3 sundir = vec3(0.f, -sinf(TWOPI*a), cosf(TWOPI*a));
		material->uniform("sundir", glm::normalize(sundir));
		material->uniform("sun_color", vec3(colors::tango::orange_7));

		float daylight = sin(TWOPI * a) * 0.5f + 0.5f; //fabs(a * 4.f - 1.f);
		float morninglight = pow(fabs(cos(TWOPI * a)), 8.0);

		// 1 = midday
		// -1 = midnight

		// red 0xffb83932

		material->uniform("sky_dark",  vec3(mix(Color(0xff000000),
		                                        Color(0xffa9b2a1), daylight)));
		material->uniform("sky_light", vec3(mix(mix(Color(0xff000000),
		                                            Color(0xff61828a), daylight),
		                                        Color(0xffb83932), morninglight)));
		//material->uniform("sky_dark", vec3(mix(Color(0xffa9b2a1),
		//                                        Color(0xff000010), time_of_day)));//colors::tango::skyblue_3));
		//material->uniform("sky_light", vec3(mix(Color(0xff61828a),
		//                                        Color(0xff202030), time_of_day)));//colors::tango::skyblue_6));
		sky.render();

		glDepthFunc(GL_LESS);
		glDepthRange(0, 1);
	}

	Window& screen;

	UiStack ui;

	bool mouse_captured;

	MatrixStack<8> projection;
	MatrixStack<32> modelview;

	Player player;

	Texture2D* cube_tex;
	Texture2D* plane_tex;
	Material* material_basic;
	Material* material_sky;
	geo::instance plane;
	vector<unique_ptr<geo::instance>> cubes;
	geo::instance sky;
	vec3 bobd;
	float a, b, c;
	bool wireframe_mode;
};

Scene* make_game(Window& screen) {
	return new Game(screen);
}
