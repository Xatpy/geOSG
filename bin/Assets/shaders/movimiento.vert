uniform int TIME_FROM_INIT;

void main()
{
	gl_Position = ftransform();
	gl_TexCoord[0].st = gl_MultiTexCoord0.st;
	float offset = sin(float(TIME_FROM_INIT) * 0.01);
	gl_TexCoord[1].t = gl_MultiTexCoord0.t;
	gl_TexCoord[1].s = gl_MultiTexCoord0.s + offset;
}
