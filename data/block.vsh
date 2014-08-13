#version 130

uniform mat4 MVP;
uniform mat3 normal_matrix;
uniform vec3 chunk_pos;

in vec3 position;
in vec3 in_normal;
in vec3 in_color;
out vec3 out_normal;
out vec3 out_color;

void main(){
	gl_Position = MVP * vec4(chunk_pos + position, 1);
	out_normal = normal_matrix * in_normal;
	out_color = in_color;
}
