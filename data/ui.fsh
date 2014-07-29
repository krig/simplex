#version 130

precision highp float;

in vec2 out_texcoord;
in vec4 out_color;
out vec4 fragment;

uniform sampler2D tex0;

void main() {
     fragment = texture(tex0, out_texcoord).rgba * out_color;
}