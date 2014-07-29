#version 130

uniform vec2 screensize;

in vec2 position;
in vec2 in_texcoord;
in vec4 in_color;
out vec2 out_texcoord;
out vec4 out_color;

void main() {
     vec2 offset = screensize/2;
     vec2 homogenouspos = position - offset;
     vec2 eyepos = homogenouspos / offset;
     gl_Position = vec4(eyepos.x, eyepos.y, 0, 1);
     out_texcoord = in_texcoord;
     out_color = in_color;
}