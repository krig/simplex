#version 130

precision highp float;

uniform vec3 fogcolor;
uniform vec3 suncolor;
uniform float fogamount;

in vec3 out_normal;
in vec3 out_color;
out vec4 fragment;

/*
vec3 applyFog( in vec3  rgb,      // original color of the pixel
               in float distance, // camera to point distance
               in vec3  rayDir,   // camera to point vector
               in vec3  sunDir )  // sun light direction
{
    float fogAmount = exp( -distance*b );
    float sunAmount = max( dot( rayDir, sunDir ), 0.0 );
    vec3  fogColor  = mix( vec3(0.5,0.6,0.7), // bluish
                           vec3(1.0,0.9,0.7), // yellowish
                           pow(sunAmount,8.0) );
    return mix( rgb, fogColor, fogAmount );
}
*/

void main(){
	float intensity = max(0.0, dot(normalize(out_normal),
	                               normalize(vec3(0.5, 1.0, 0.5))));
	fragment = vec4(out_color * (0.2 + intensity), 1.0);
}
