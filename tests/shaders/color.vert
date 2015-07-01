attribute vec2 aPosition;
attribute vec4 aColor;

varying vec4 vColor;

void main(void)
{
	vColor = aColor;
	gl_Position = vec4(aPosition, 0.0, 1.0);
}