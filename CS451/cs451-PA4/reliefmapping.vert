//
// TODO: Implement relief mapping. This should add on to the normal mapping shader: normalmapping.vert/frag
//

void main()
{
	gl_FrontColor =  gl_Color;
	gl_Position = ftransform();
} 