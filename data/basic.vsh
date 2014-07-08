#version 130

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

in vec3 position;
in vec3 in_normal;
out vec3 out_normal;

void main(){
	gl_Position = projection * view * model * vec4(position, 1.0);
	out_normal = (model * vec4(in_normal, 0.0)).xyz;
}
