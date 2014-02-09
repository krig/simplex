#include "common.hpp"
#include "rand.hpp"
#include <math.h>

namespace {
	const float GRAVITY = 9.81f * 1.2f;//1.33f;
	const float DT = 1.f / 60.f;
	const float PI = 3.14159265358979323846f;
	const float SCR_W = 270.f;
	const float SCR_H = 480.f;

	struct game_data {
		game_data() {
			running = true;
		}

		~game_data() {
		}

		sdl::screen screen;
		sdl::sprite bg;
		sdl::sprite hills;
		sdl::sprite cloud;
		sdl::numbers npass;
		bool running;
	};

	struct player_data {
		float x;
		float y;
		float velocity;

		sdl::animated_sprite img;

		player_data() {
			x = SCR_W*0.5f;
			y = SCR_H*0.5f;
			velocity = 0;
		}
	};

	struct pipes_data {
		
	};

	game_data game;
	player_data player;
	pipes_data pipes;

	int num_pass = 0;
	float hills_pos = 0;
	float cloud_pos = 0;

	void init() {
		game.screen.create((int)SCR_W, (int)SCR_H, 540, 960);
		game.bg.load(game.screen, "data/bg0.png");
		game.hills.load(game.screen, "data/bg1.png");
		game.cloud.load(game.screen, "data/cloud.png");
		game.npass.load(game.screen, "data/numbers.png");
		player.img.load(game.screen, "data/bird.png");
		player.img.init_anim(3, 4);
	}

	void player_jump();

	void handle_event(SDL_Event* e) {
		if (e->type == SDL_QUIT) {
			game.running = false;
		}
		else if (e->type == SDL_KEYDOWN) {
			if (e->key.keysym.sym == SDLK_ESCAPE) {
				game.running = false;
			}
			else if (e->key.keysym.sym == SDLK_SPACE) {
				player_jump();
			}
		}
	}

	void update_background() {
		hills_pos -= 32.0f * DT;
		if (hills_pos <= -SCR_W)
			hills_pos += SCR_W;
		cloud_pos -= 10.0f * DT;
		if (cloud_pos <= -SCR_W)
			cloud_pos += SCR_W;
	}

	void update_player() {
		player.velocity -= GRAVITY * DT;
		if (player.velocity < -GRAVITY)
			player.velocity = -GRAVITY;
		if (player.velocity > GRAVITY)
			player.velocity = GRAVITY;

		if (player.velocity >= 0.f)
			player.img.ticks_per_frame = 2;
		else
			player.img.ticks_per_frame = 6;
		player.img.update();

		player.y -= player.velocity;

		if (player.y > SCR_H + player.img.frame_h() * 0.5f) {
			player.y = SCR_H * 0.5f;
			player.velocity = 0.f;
			num_pass = 0;
		}
	}

	void player_jump() {
		num_pass += 1;
		if (player.y > player.img.frame_h() * 0.5f)
			player.velocity = 0.5f * GRAVITY;
	}

	void draw_background() {
		game.bg.draw(game.screen);
		game.cloud.draw(game.screen, sdl::rect(cloud_pos + SCR_W, SCR_H - game.cloud.h, game.cloud.w, game.cloud.h));
		game.cloud.draw(game.screen, sdl::rect(cloud_pos, SCR_H - game.cloud.h, game.cloud.w, game.cloud.h));
		game.hills.draw(game.screen, sdl::rect(hills_pos + SCR_W, SCR_H - game.hills.h, game.hills.w, game.hills.h));
		game.hills.draw(game.screen, sdl::rect(hills_pos, SCR_H - game.hills.h, game.hills.w, game.hills.h));
	}

	void draw_player() {
		float x = player.x - player.img.w * 0.5f;
		float y = player.y - player.img.frame_h() * 0.5f;
		float angle = -player.velocity * 360.f / (PI * 8.f);
		if (angle > 85.f)
			angle = 85.f;
		else if (angle < -30.f)
			angle = -30.f;
		sdl::point p(player.img.w * 0.33f, player.img.frame_h() * 0.5f);
		player.img.draw_angle(game.screen, sdl::rect(x, y, player.img.w, player.img.frame_h()), angle, &p);
	}

	void tick() {
		update_background();
		update_player();
	}

	void render() {
		draw_background();
		draw_player();

		game.npass.draw(game.screen, num_pass, sdl::point(SCR_W/2, 20));

		game.screen.present();
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
			sdl::delay_to_fps(counter, (Uint32)(DT * 1000.0));
		}
	}

}

int main(int argc, char* argv[]) {
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
