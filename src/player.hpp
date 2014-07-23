#pragma once

struct Player {

	Player() {
		pos = vec3(0, 0, 0);
		yaw = pitch = 0;
		offset = vec3(0, 0, 0);
		bob = vec3(0, 0, 0);
		velocity = vec3(0, 0, 0);
	}

	void move(const glm::vec3& dir) {
		mat4 ori = glm::rotate(mat4(), yaw, vec3(0.f, 1.f, 0.f));
		velocity += vec3(ori * vec4(dir, 1.f));
	}

	void look(float by, float x) {
		yaw -= by;
		while (yaw < 0.f)
			yaw += TWOPI;
		while (yaw > TWOPI)
			yaw -= TWOPI;

		pitch -= x;
		if (pitch > HALFPI)
			pitch = HALFPI;
		if (pitch < -HALFPI)
			pitch = -HALFPI;
	}

	void jump() {
		if (!inair) {
			inair = true;
			velocity += vec3(0.f, 10.f, 0.f);
		}
	}

	void tick(double dt) {
		float MAX_VEL = 5.f;
		velocity = glm::clamp(velocity, -MAX_VEL, MAX_VEL);
		pos += velocity * (float)dt;
		if (pos.y < 0.f) {
			inair = false;
			pos.y = 0.f;
		}
		velocity.x *= 0.8f;
		velocity.z *= 0.8f;
		velocity.y -= 9.8f * 1.3f * dt;
	}

	vec3 velocity;
	vec3 pos;
	float yaw; // look angle around Y axis (radians), yaw
	float pitch; // look angle around X axis (radians)
	vec3 offset; // camera offset from player position
	vec3 bob; // view bob (while walking / running)
	bool inair;
};
