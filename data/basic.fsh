#version 130

precision highp float;

in vec3 out_normal;
in vec2 out_texcoord;
in vec3 out_color;
out vec4 fragment;

uniform sampler2D tex0;

void main(){
	float intensity = max(0.0, dot(normalize(out_normal), normalize(vec3(0.5, 1.0, 0.5))));
	fragment = vec4(texture(tex0, out_texcoord).rgb * out_color * (0.2 + intensity), 1.0);
}
