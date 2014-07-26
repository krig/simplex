#pragma once

struct Scene {
	Scene() : running(true) {}
	virtual ~Scene() {}
	virtual void init() = 0;
	virtual void update(double dt) = 0;
	virtual void handle_event(SDL_Event* e) = 0;
	virtual void render() = 0;

	bool running;
};
