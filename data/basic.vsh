#version 130

uniform mat4 projection;
uniform mat4 camera;
uniform mat4 object;

in vec3 position;
in vec2 in_color;
out vec3 out_color;

void main(){
	gl_Position = projection * camera * object * vec4(position, 1.0);
	out_color = vec3(in_color, 0.222);
}
