#version 130

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

in vec3 position;
in vec3 in_normal;
in vec2 in_texcoord;
in vec3 in_color;
out vec3 out_normal;
out vec2 out_texcoord;
out vec3 out_color;

void main(){
	gl_Position = projection * view * model * vec4(position, 1);
	out_normal = (model * vec4(in_normal, 0)).xyz;
	out_texcoord = in_texcoord;
	out_color = in_color;
}
