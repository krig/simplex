#pragma once

struct Player {

	Player() : pos(0, 0, 0), angle(0) {
	}

	void move(const glm::vec3& dir) {
		mat4 ori = glm::rotate(mat4(), angle, vec3(0.f, 1.f, 0.f));
		pos += vec3(ori * vec4(dir, 1.f));
	}

	void rotate(float by) {
		angle -= by;
		while (angle < 0.f)
			angle += TWOPI;
		while (angle > TWOPI)
			angle -= TWOPI;
	}

	void jump() {
	}

	vec3 pos;
	float angle; // look angle around Y axis (radians), yaw
};
