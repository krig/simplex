#pragma once

struct Player {

	Player() {
		pos = vec3(0, 0, 0);
		yaw = pitch = 0;
		offset = vec3(0, 0, 0);
		bob = vec3(0, 0, 0);
		velocity = vec3(0, 0, 0);
		jumpcount = 0.f;
		onground = true;
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
		if (onground && jumpcount <= 0.f) {
			jumpcount = 0.2f;
			velocity.y += 8.f + 2.f * std::min(1.f, (float)abs(glm::length(velocity)));
			onground = false;
		}
	}

	void tick(double dt) {
		jumpcount = std::max(0.f, jumpcount - (float)dt);
		float MAX_VEL = 54.f; // terminal velocity
		velocity = glm::clamp(velocity, -MAX_VEL, MAX_VEL);
		pos += velocity * (float)dt;
		if (pos.y < 0.f) {
			velocity.y = 0.f;
			pos.y = 0.f;
			onground = true;
		}
		if (onground) {
			velocity.x *= 0.8f;
			velocity.z *= 0.8f;
		} else {
			velocity.x *= 0.83f;
			velocity.z *= 0.83f;
		}
		velocity.y -= 9.8f * 3.33f * dt;
	}

	vec3 velocity;
	vec3 pos;
	float yaw; // look angle around Y axis (radians), yaw
	float pitch; // look angle around X axis (radians)
	vec3 offset; // camera offset from player position
	vec3 bob; // view bob (while walking / running)
	float jumpcount;
	bool onground;
};
