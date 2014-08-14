#version 130

precision highp float;

in vec3 out_normal;
in vec2 out_texcoord;
in vec3 out_color;
in float out_depth;
out vec4 fragment;

uniform sampler2D tex0;

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

vec3 fog(vec3 color, vec3 fcolor, float depth, float density){
	const float e = 2.71828182845904523536028747135266249;
    float f = pow(e, -pow(depth*density, 2));
    return mix(fcolor, color, f);
}

void main(){
	float intensity = max(0.0, dot(normalize(out_normal), normalize(vec3(0.5, 1.0, 0.5))));
	vec3 basecolor = texture(tex0, out_texcoord).rgb * out_color * (0.2 + intensity);

	vec3 fogged = fog(basecolor, vec3(0.5, 0.6, 0.7), out_depth, 0.015);
	fragment = vec4(fogged, 1.0);
}
