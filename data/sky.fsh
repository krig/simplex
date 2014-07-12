#version 130

precision highp float;

uniform vec3 sky_dark;
uniform vec3 sky_light;

in float gradient;
out vec4 fragment;

void main() {
	fragment = vec4(mix(sky_light, sky_dark, clamp(gradient, 0, 1)), 1);
}
