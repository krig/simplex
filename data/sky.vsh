#version 130

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

in vec3 position;
in vec3 in_normal;
in vec2 in_texcoord;
in vec3 in_color;
out float gradient;

void main(){
    gradient = position.y;
	gl_Position = projection * view * model * vec4(position, 1.0);
}
