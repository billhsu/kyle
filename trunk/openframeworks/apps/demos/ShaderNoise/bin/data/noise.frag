const int octaves = 8;
const float pi = asin(1.) * 2.;

uniform float seed;
uniform float[octaves] weights;
uniform float[octaves] scaling;
uniform float normalization;

float rand(vec2 coord) {
	return fract(sin(dot(coord.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 cosMix(vec2 x, vec2 y, float a) {
	return mix(x, y, (1. - cos(a * pi)) / 2.);
}

float cosMix(float x, float y, float a) {
	return mix(x, y, (1. - cos(a * pi)) / 2.);
}

float bilinearRand(vec2 pos) {
	vec2 f = fract(pos);
	vec2 left = vec2(
		rand(floor(pos)),
		rand(floor(pos + vec2(0, 1))));
	vec2 right = vec2(
		rand(floor(pos + vec2(1, 0))),
		rand(floor(pos + vec2(1, 1))));
	vec2 vert = cosMix(left, right, f.x);
	return cosMix(vert.x, vert.y, f.y);
}

float noise(vec2 pos) {
	float total = 0.;
	for(int i = 0; i < octaves; i++)
		total += weights[i] * bilinearRand((pos + seed * float(i + 1)) * scaling[i]);
	return total * normalization;
}

void main(void){
	vec2 pos = gl_TexCoord[0].xy;
	gl_FragColor = vec4(vec3(noise(pos)), 1.);
}
