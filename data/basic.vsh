#version 130

uniform mat4 MVP;
uniform mat3 normal_matrix;

in vec3 position;
in vec3 in_normal;
in vec2 in_texcoord;
in vec3 in_color;
out vec3 out_normal;
out vec2 out_texcoord;
out vec3 out_color;

void main(){
	gl_Position = MVP * vec4(position, 1);
	out_normal = normal_matrix * in_normal;
	out_texcoord = in_texcoord;
	out_color = in_color;
}
