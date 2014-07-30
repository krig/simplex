#version 130

precision highp float;

in vec2 out_texcoord;
in vec4 out_color;
out vec4 fragment;

uniform sampler2D tex0;

void main() {
     float a = texture(tex0, out_texcoord).r;
     fragment = out_color * vec4(1, 1, 1, a);
}