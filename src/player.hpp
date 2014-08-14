#pragma once

struct Player {

	Player();
	void move(const glm::vec3& dir);
	void look(float by, float x);
	void jump();
	void duck(bool ducking);
	void update(double dt);
	void toggle_freecam();

	// calculate world -> view matrix
	mat4 make_view_matrix();

	// calculate world -> view matrix with no translation
	mat4 make_sky_view_matrix();

	vec3 velocity;
	vec3 pos;
	float yaw; // look angle around Y axis (radians), yaw
	float pitch; // look angle around X axis (radians)
	vec3 offset; // camera offset from player position
	vec3 bob; // view bob (while walking / running)
	float jumpcount;
	bool onground;
	bool ducking;
	bool freecam;
};
