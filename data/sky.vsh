#version 130

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

in vec3 position;
in vec3 in_normal;
in vec3 in_color;
out float gradient;

void main(){
	gl_Position = projection * view * model * vec4(position, 1.0);
    gradient = position.y / 10.0;
}
