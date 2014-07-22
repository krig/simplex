#include "common.hpp"
#include "render.hpp"

SDL::SDL() {
	_sdl = _mix = _img = false;
}

void SDL::init() {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		throw error("SDL_Init failed: %s", SDL_GetError());
	_sdl = true;

	if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) < 0)
		throw error("IMG_Init failed: %s", IMG_GetError());
	_img = true;

	if (Mix_Init(MIX_INIT_OGG) < 0)
		throw error("Mix_Init failed: %s", Mix_GetError());
	_mix = true;
}

SDL::~SDL() {
	if (_mix) Mix_Quit();
	if (_img) IMG_Quit();
	if (_sdl) SDL_Quit();
}


void Window::create(const char* title, int w, int h, int winw, int winh) {
	window = SDL_CreateWindow(title,
	                          SDL_WINDOWPOS_UNDEFINED,
	                          SDL_WINDOWPOS_UNDEFINED,
	                          winw,
	                          winh,
	                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == nullptr) {
		throw error("%s", SDL_GetError());
	}
	context = SDL_GL_CreateContext(window);
	if (context == nullptr) {
		throw error("%s", SDL_GetError());
	}
	SDL_GL_MakeCurrent(window, context);

	glewExperimental = GL_TRUE;
	auto glewrc = glewInit();
	if (glewrc != GLEW_OK) {
		throw error("glew error %s \n", glewGetErrorString(glewrc));
	}
	int major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	printf("GL %d.%d\n", major, minor);

	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void Window::present() {
	SDL_GL_SwapWindow(window);
}

SDL_Point Window::get_size() const {
	SDL_Point sz;
	SDL_GetWindowSize(window, &sz.x, &sz.y);
	return sz;
}
