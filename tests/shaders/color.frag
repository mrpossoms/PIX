varying vec4 vColor;

uniform vec4  uColor;

void main(void){
	gl_FragColor = vColor * uColor;
}