        #version 130

precision highp float;

uniform vec3 sky_dark;
uniform vec3 sky_light;

in float gradient;
out vec4 fragment;

void main() {
	fragment = vec4(mix(sky_dark, sky_light, smoothstep(0, 10.f, gradient)), 1);
}
