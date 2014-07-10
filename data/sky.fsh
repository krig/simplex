#version 130

precision highp float;

uniform vec3 sky_dark;
uniform vec3 sky_light;

in float gradient;
out vec4 fragment;

void main(){
	vec3 light = sky_dark * (1.0 - gradient) + sky_light * gradient;
	fragment = vec4(light, 1.0);
}
