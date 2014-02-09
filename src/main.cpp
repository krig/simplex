#include "common.hpp"
#include "rand.hpp"
#include <math.h>

namespace {
	const float GRAVITY = 9.81f * 0.5f;//1.33f;
	const float DT = 1.f / 60.f;
	const float PI = 3.14159265358979323846f;
	const float SCR_W = 135.f;
	const float SCR_H = 240.f;

	struct Bird {
		Bird() {
			x = SCR_W*0.5f;
			y = SCR_H*0.5f;
			velocity = 0;
			pos = 0;
			alive = true;
		}

		void init(sdl::screen& screen, sdl::tileset* tiles) {
			this->tiles = tiles;
			flight.frames.push_back(0);
			flight.frames.push_back(1);
			flight.frames.push_back(2);
			flight.type = sdl::animation::looping;
			flight.speed = 1.f;
			flight.start();
		}

		void tick() {
			pos++;

			velocity -= GRAVITY * DT;
			if (velocity < -GRAVITY)
				velocity = -GRAVITY;
			if (velocity > GRAVITY)
				velocity = GRAVITY;

			if (velocity >= 0.f)
				flight.speed = 12.f;
			else
				flight.speed = 6.f;
			flight.update(DT);

			y -= velocity;

			if (alive && (y > SCR_H - (float)tiles->tilesize)) {
				//y = SCR_H * 0.5f;
				velocity = 0.5f * GRAVITY;
				//velocity = 0.f;
				//pos = 0;
				alive = false;
			}
		}

		void render(sdl::screen& screen) {
			float w = (float)tiles->tilesize;
			float h = (float)tiles->tilesize;
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
			tiles->draw_angle(screen, frame, sdl::point(x, y), angle, &p);
		}

		void jump() {
			if (!alive) {
				if (y > SCR_H + (float)tiles->tilesize * 0.5f) {
					alive = true;
					y = SCR_H * 0.5f;
					velocity = 0.f;
					pos = 0;
				}
				return;
			}
			if (y > (float)tiles->tilesize * 0.5f)
				velocity = 0.5f * GRAVITY;
		}

		float x;
		float y;
		float velocity;
		uint32_t pos;
		bool alive;
		sdl::tileset* tiles;
		sdl::animation flight;
	};

	struct Background {
		Background() {
		}

		void init(sdl::screen& screen, sdl::tileset* tiles) {
			this->tiles = tiles;
			clouds_pos = 0;
			hills_pos = 0;
			ground_pos = 0;
		}

		void tick() {
			ground_pos += 42.f * DT;
			if (ground_pos > (float)tiles->tilesize)
				ground_pos -= (float)tiles->tilesize;
			hills_pos += 22.f * DT;
			if (hills_pos > (float)tiles->tilesize)
				hills_pos -= (float)tiles->tilesize;
			clouds_pos += 1.f * DT;
			if (clouds_pos > (float)tiles->tilesize)
				clouds_pos -= (float)tiles->tilesize;
		}

		void render(sdl::screen& screen) {
			tiles->draw(screen, 14, sdl::rect(0, 0, SCR_W, SCR_H));
			int ntiles = ((int)SCR_W / tiles->tilesize) + 2;
			for (int i = 0; i < ntiles; ++i) {
				tiles->draw(screen, 10, sdl::point((float)(i*tiles->tilesize) - clouds_pos, SCR_H - 2*tiles->tilesize));
			}
			for (int i = 0; i < ntiles; ++i) {
				tiles->draw(screen, 8, sdl::point((float)(i*tiles->tilesize) - hills_pos, SCR_H - 2*tiles->tilesize));
				tiles->draw(screen, 9, sdl::point((float)((i + 1)*tiles->tilesize) - hills_pos, SCR_H - 2*tiles->tilesize));
				tiles->draw(screen, 13, sdl::point((float)(i*tiles->tilesize) - ground_pos, SCR_H - tiles->tilesize));
			}
		}

		sdl::tileset* tiles;
		float clouds_pos;
		float hills_pos;
		float ground_pos;
	};

	struct Obstacles {
		struct Obstacle {
			double offset;
			int gap;
		};

		Obstacles() {
		}

		void init(sdl::screen& screen, sdl::tileset* tiles) {
			this->tiles = tiles;
			double last_offset = 120.f;
			for (size_t i = 0; i < ASIZE(items); ++i) {
				items[i].offset = last_offset;
				items[i].gap = util::randint() % 4;
				last_offset += 80.f + (float)(util::randint() % 120);
			}
		}

		void tick() {
		}

		void render(sdl::screen& screen) {
			for (size_t i = 0; i < ASIZE(items); ++i) {
				int nblocks = 4;
				for (int i = 0; i < nblocks; ++i) {
					//tiles->draw(screen, 4, sdl::point((float)(i*tiles->tilesize) - hills_pos, SCR_H - 2*tiles->tilesize));
				}
			}
		}

		sdl::tileset* tiles;
		Obstacle items[4];
	};

	struct Flappy {
		Flappy() {
			running = true;
		}

		void init() {
			screen.create((int)SCR_W, (int)SCR_H, 540, 960);
			tiles.load(screen, "data/birds-tiles.png", 16);
			npass.load(screen, "data/numbers.png");
			bird.init(screen, &tiles);
			bg.init(screen, &tiles);
			obstacles.init(screen, &tiles);
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
					bird.jump();
				}
			}
		}

		void tick() {
			bg.tick();
			obstacles.tick();
			bird.tick();

			// check_collisions
		}

		void render() {
			bg.render(screen);
			obstacles.render(screen);
			bird.render(screen);
			npass.draw(screen, 4, sdl::point(SCR_W/2, 20));

			if (!bird.alive) {
				tiles.draw(screen, sdl::rect(0, 32, 40, 32),
				           sdl::rect(SCR_W*0.5f - 20, SCR_H*0.5f - 16, 40, 32));
			}

			screen.present();
		}

		sdl::screen screen;
		sdl::tileset tiles;
		sdl::numbers npass;
		Bird bird;
		Background bg;
		Obstacles obstacles;
		bool running;
	};

	Flappy game;

	void mainloop() {
		SDL_Event event;
		Uint32 counter = 0;
		game.init();
		game.render();
		while (game.running) {
			counter = SDL_GetTicks();
			while (SDL_PollEvent(&event)) {
				game.handle_event(&event);
			}
			game.tick();
			game.render();
			sdl::delay_to_fps(counter, (Uint32)(DT * 1000.0));
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
