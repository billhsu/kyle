#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect displacementMap;

varying vec4 color;

const float depthScaling = 255. * 2.5;

void main() {
	color = texture2DRect(displacementMap, gl_MultiTexCoord0.xy);
	vec4 pos = gl_Vertex;
	pos.z += color.a * depthScaling;
	gl_Position = gl_ModelViewProjectionMatrix * pos;
}
