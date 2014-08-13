#version 130

precision highp float;

uniform vec3 sundir;
uniform vec3 sun_color;
uniform vec3 sky_dark;
uniform vec3 sky_light;

in float gradient;
in vec3 out_normal;
out vec4 fragment;

void main() {
	float sunamt = pow(max(0.0, dot(out_normal, sundir)), 6.0);
	vec3 skybase = mix(sky_light, sky_dark, smoothstep(0, 5.f, gradient));
	fragment = vec4(mix(skybase, sun_color, sunamt), 1);
}
