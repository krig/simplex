#version 130

precision highp float;

in vec3 out_normal;
out vec4 fragment;
void main(){
	float intensity = max(0.0, dot(normalize(out_normal), normalize(vec3(0.3, 1.0, 0.2))));
	fragment = vec4(vec3(0.4, 1.0, 0.1) * (0.2 + intensity), 1.0);
}
