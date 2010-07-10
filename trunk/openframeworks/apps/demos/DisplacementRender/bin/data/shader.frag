varying vec4 color;

void main(void) {
	if(color.a == 0.)
		discard;
	gl_FragColor = vec4(color.rgb, 1.);
}
