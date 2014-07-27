#include "common.hpp"
#include "render.hpp"
#include "player.hpp"

namespace {
    mat4 fps_view(vec3 eye, float pitch, float yaw) {
        float cosPitch = cosf(pitch);
        float sinPitch = sinf(pitch);
        float cosYaw = cosf(yaw);
        float sinYaw = sinf(yaw);
        vec3 xaxis(cosYaw, 0, -sinYaw);
        vec3 yaxis(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
        vec3 zaxis(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

        return mat4 {
            vec4(xaxis.x, yaxis.x, zaxis.x, 0),
            vec4(xaxis.y, yaxis.y, zaxis.y, 0),
            vec4(xaxis.z, yaxis.z, zaxis.z, 0),
            vec4(-glm::dot( xaxis, eye ), -glm::dot( yaxis, eye ), -glm::dot( zaxis, eye ), 1)
        };
    }

}

Player::Player() {
    pos = vec3(0, 0, 0);
    yaw = pitch = 0;
    offset = vec3(0, 0, 0);
    bob = vec3(0, 0, 0);
    velocity = vec3(0, 0, 0);
    jumpcount = 0.f;
    onground = true;
}

void Player::move(const glm::vec3& dir) {
	if (onground) {
		mat4 ori = glm::rotate(mat4(), yaw, vec3(0.f, 1.f, 0.f));
		velocity += vec3(ori * vec4(dir, 1.f));
	}
}

void Player::look(float by, float x) {
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

void Player::jump() {
    if (onground && jumpcount <= 0.f) {
        jumpcount = 0.2f;
        velocity.y += 8.f + 2.f * std::min(1.f, (float)abs(glm::length(velocity)));
        onground = false;
    }
}

void Player::update(double dt) {
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
	    //velocity.x *= 0.83f;
	    //velocity.z *= 0.83f;
    }
    velocity.y -= 9.8f * 3.33f * dt;
}

// calculate world -> view matrix
mat4 Player::make_view_matrix() {
    return fps_view(pos + offset + bob, pitch, yaw);
}

// calculate world -> view matrix with no translation
mat4 Player::make_sky_view_matrix() {
    return fps_view(vec3(0, 0, 0), pitch, yaw);
}
