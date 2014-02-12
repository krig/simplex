#include "common.hpp"
#include "rand.hpp"
#include <deque>
#include <algorithm>
#include <math.h>

namespace {
	const float GRAVITY = 15.f;//1.33f;
	const float FLAP_FORCE = 4.666f;
	const double TARGET_FPS = 60.0;
	const float PI = 3.14159265358979323846f;
	const float SCR_W = 135.f;
	const float SCR_H = 240.f;
	const float BIRD_POS = SCR_W*0.25f;
	const float GROUND_SPEED = 120.f;
	const float HILLS_SPEED = 30.f;
	const float CLOUDS_SPEED = 8.f;
	const int GAP_HEIGHT = 4;
	const float MIN_PIPE_DIST = SCR_W * 0.75f;
	const float RND_PIPE_DIST = SCR_W;

	sdl::tileset* g_tiles;
	double last_pipe_offset = 0;

	struct Bird {
		Bird() {
			x = BIRD_POS;
			revive();
		}

		void init(sdl::screen& screen) {
			flight.frames.push_back(0);
			flight.frames.push_back(1);
			flight.frames.push_back(2);
			flight.type = sdl::animation::looping;
			flight.speed = 1.f;
			flight.start();
		}

		void tick(double dt) {
			velocity -= GRAVITY * dt;
			if (velocity < -GRAVITY)
				velocity = -GRAVITY;
			if (velocity > GRAVITY)
				velocity = GRAVITY;
			y -= velocity;

			if (velocity >= 0.f)
				flight.speed = 12.f;
			else
				flight.speed = 6.f;
			flight.update(dt);


			if (alive && (y > SCR_H - (float)g_tiles->tilesize))
				die();
		}

		void die() {
			velocity = FLAP_FORCE;
			alive = false;
		}

		void render(sdl::screen& screen) {
			float w = (float)g_tiles->tilesize;
			float h = (float)g_tiles->tilesize;
			float x = this->x - w * 0.5f;
			float y = this->y - h * 0.5f;
			float angle = -velocity * 360.f / (PI * 8.f);
			if (angle > 85.f)
				angle = 85.f;
			else if (angle < -30.f)
				angle = -30.f;
			sdl::point p(w * 0.33f, h * 0.5f);
			int frame = flight.current();
			if (!alive)
				frame = 3;
			g_tiles->draw_angle(screen, frame, sdl::point(x, y), angle, &p);
		}

		bool flap() {
			if (!alive)
				return can_revive();
			if (y > (float)g_tiles->tilesize * 0.5f)
				velocity = FLAP_FORCE;
			return false;
		}

		bool can_revive() {
			return y > SCR_H + (float)g_tiles->tilesize * 0.5f;
		}

		void revive() {
			alive = true;
			y = SCR_H * 0.5f;
			velocity = 0.f;
			num_passed = 0;
		}

		float x;
		float y;
		float velocity;
		int num_passed;
		bool alive;
		sdl::animation flight;
	};

	struct Background {
		Background() {
		}

		void init(sdl::screen& screen) {
			clouds_pos = 0;
			hills_pos = 0;
			ground_pos = 0;
		}

		void tick(double dt) {
			ground_pos += GROUND_SPEED * dt;
			if (ground_pos > (float)g_tiles->tilesize)
				ground_pos -= (float)g_tiles->tilesize;
			hills_pos += HILLS_SPEED * dt;
			if (hills_pos > (float)g_tiles->tilesize)
				hills_pos -= (float)g_tiles->tilesize;
			clouds_pos += CLOUDS_SPEED * dt;
			if (clouds_pos > (float)g_tiles->tilesize)
				clouds_pos -= (float)g_tiles->tilesize;
		}

		void render(sdl::screen& screen) {
			g_tiles->draw(screen, 14, sdl::rect(0, 0, SCR_W, SCR_H));
			int ntiles = ((int)SCR_W / g_tiles->tilesize) + 2;
			for (int i = 0; i < ntiles; ++i) {
				g_tiles->draw(screen, 10, sdl::point((float)(i*g_tiles->tilesize) - clouds_pos, SCR_H - 2*g_tiles->tilesize));
			}
			for (int i = 0; i < ntiles; ++i) {
				g_tiles->draw(screen, 8, sdl::point((float)(i*g_tiles->tilesize) - hills_pos, SCR_H - 2*g_tiles->tilesize));
				g_tiles->draw(screen, 9, sdl::point((float)((i + 1)*g_tiles->tilesize) - hills_pos, SCR_H - 2*g_tiles->tilesize));
				g_tiles->draw(screen, 13, sdl::point((float)(i*g_tiles->tilesize) - ground_pos, SCR_H - g_tiles->tilesize));
			}
		}

		float clouds_pos;
		float hills_pos;
		float ground_pos;
	};

	struct Pipe {
		Pipe() {
			last_pipe_offset = last_pipe_offset + MIN_PIPE_DIST + util::randint() % (int)RND_PIPE_DIST;
			offset = last_pipe_offset;
			int total_height = SCR_H / g_tiles->tilesize;
			gap = 1 + util::randint() % (total_height - 2 - GAP_HEIGHT);
			passed = 0;
			outside_screen = false;
		}

		void tick(double dt) {
			offset -= GROUND_SPEED * dt;
			outside_screen = offset < -g_tiles->tilesize;
			//LOG_TRACE("%f %g", offset, dt);
		}

		void render(sdl::screen& screen) {
			if (offset - (float)g_tiles->tilesize > SCR_W || outside_screen)
				return;

			int total_height = SCR_H / g_tiles->tilesize + 1;
			int top_height = total_height - gap - GAP_HEIGHT - 2;
			int bottom_height = gap;
			// render below gap
			for (int i = 0; i < bottom_height; ++i) {
				g_tiles->draw(screen, 4, sdl::point(offset, (top_height + GAP_HEIGHT + i) * g_tiles->tilesize));
			}
			// render above gap
			for (int i = 0; i < top_height; ++i) {
				g_tiles->draw(screen, 4, sdl::point(offset, i * g_tiles->tilesize));
			}
		}

		bool collide(const Bird& bird) {
			using sdl::rect;
			float bx = bird.x - (float)g_tiles->tilesize * 0.5f;
			float by = bird.y - (float)g_tiles->tilesize * 0.5f;
			if (bx >= offset)
				passed++;
			int total_height = SCR_H / g_tiles->tilesize + 1;
			int top_height = total_height - gap - GAP_HEIGHT - 2;
			int bottom_height = gap;

			rect top(offset, -SCR_H,
			         g_tiles->tilesize,
			         SCR_H + top_height * g_tiles->tilesize);
			rect bottom(offset, (top_height + GAP_HEIGHT) * g_tiles->tilesize,
			            g_tiles->tilesize,
			            bottom_height * g_tiles->tilesize);
			int inset = 4;
			rect b(bx + inset/2, by + inset/2, g_tiles->tilesize - inset, g_tiles->tilesize - inset);
			return bottom.has_intersection(b) || top.has_intersection(b);
		}

		double offset;
		bool outside_screen;
		int passed;
		int gap;
	};

	struct Flappy {
		Flappy() {
			running = true;
		}

		void init() {
			screen.create("flappy beard", (int)SCR_W, (int)SCR_H, 540, 960);

			Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 512);
			Mix_AllocateChannels(3);

			sfxflap.load("data/flap.wav");
			sfxgate.load("data/gate.wav");
			sfxfail.load("data/fail.wav");
			beardsong.load("data/beardsong.ogg");

			tiles.load(screen, "data/birds-tiles.png", 16);
			g_tiles = &tiles;
			npass.load(screen, "data/numbers.png");
			bird.init(screen);
			bg.init(screen);

			reset_game();

			beardsong.play();
		}

		void play_flap() {
			sfxflap.play(0);
		}

		void play_gate() {
			sfxgate.play(1);
		}

		void play_fail() {
			sfxfail.play(2);
		}

		void handle_event(SDL_Event* e) {
			if (e->type == SDL_QUIT) {
				running = false;
			}
			else if (e->type == SDL_KEYDOWN) {
				if (e->key.keysym.sym == SDLK_ESCAPE) {
					running = false;
				}
				else if (e->key.keysym.sym == SDLK_SPACE) {
					if (bird.flap() && !bird.alive) {
						reset_game();
						bird.revive();
					}
					else if (bird.alive) {
						play_flap();
					}
				}
			}
		}

		void reset_game() {
			pipes.clear();
			last_pipe_offset = SCR_W;
			for (int i = 0; i < 2; ++i) {
				pipes.push_back(Pipe());
			}
		}

		void tick(double dt) {
			bool was_alive = bird.alive;
			if (bird.alive) {
				bg.tick(dt);
				for (auto& pipe : pipes)
					pipe.tick(dt);
			}
			bird.tick(dt);

			if (was_alive && !bird.alive)
				play_fail();

			if (bird.alive) {
				for (auto& pipe : pipes) {
					if (pipe.collide(bird)) {
						bird.die();
						play_fail();
						break;
					}
					else if (pipe.passed == 1) {
						pipe.passed++;
						bird.num_passed++;
						play_gate();
					}
				}
			}
			if (pipes.front().outside_screen) {
				last_pipe_offset = pipes.back().offset;
				pipes.pop_front();
				pipes.push_back(Pipe());
			}
		}

		void render() {
			bg.render(screen);
			for (auto& pipe : pipes)
				pipe.render(screen);
			bird.render(screen);
			npass.draw(screen, bird.num_passed, sdl::point(SCR_W/2, 20));

			if (!bird.alive) {
				tiles.draw(screen, sdl::rect(0, 32, 40, 32),
				           sdl::rect(SCR_W*0.5f - 20, SCR_H*0.5f - 16, 40, 32));

				if (bird.can_revive()) {
					int BLINK_FREQ = TARGET_FPS;
					ready_blink++;
					if (ready_blink < BLINK_FREQ/2) {
						tiles.draw(screen, sdl::rect(48, 32, 58, 12),
						           sdl::rect(SCR_W*0.5f - 29, SCR_H*0.5f + 32, 58, 12));
					}
					if (ready_blink > BLINK_FREQ) {
						ready_blink = 0;
					}
				}
			}

			screen.present();
		}

		sdl::screen screen;
		sdl::tileset tiles;
		sdl::numbers npass;

		sdl::sfx sfxflap;
		sdl::sfx sfxgate;
		sdl::sfx sfxfail;
		sdl::music beardsong;

		Bird bird;
		Background bg;
		std::deque<Pipe> pipes;
		int num_passed;
		int ready_blink;
		bool running;
	};

	Flappy game;

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

		//State previous;
		//State current;

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
				//previousState = currentState;
				//integrate( currentState, t, dt );
				while (SDL_PollEvent(&event)) {
					game.handle_event(&event);
				}
				game.tick(dt);

				t += dt;
				accumulator -= dt;
			}

			//const double alpha = accumulator / dt;

			game.render();
			//State state = currentState*alpha + previousState * ( 1.0 - alpha );

			//render( state );
		}
/*
		SDL_Event event;
		game.init();
		game.render();
		Uint64 t = 0.0;
		Uint64 freq = SDL_GetPerformanceFrequency() / (Uint64)1000;
		while (game.running) {
			t = SDL_GetPerformanceCounter();
			while (SDL_PollEvent(&event)) {
				game.handle_event(&event);
			}
			game.tick(1.0 / TARGET_FPS);
			game.render();
			t = SDL_GetPerformanceCounter() - t;
			Uint64 ms = t / freq;
			sdl::delay_to_fps((Uint32)ms, (Uint32)(1000.0 / TARGET_FPS));
		}
*/
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
